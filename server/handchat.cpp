/*__            ___                 ***************************************
/   \          /   \          Copyright (c) 1996-2020 Freeciv21 and Freeciv
\_   \        /  __/          contributors. This file is part of Freeciv21.
 _\   \      /  /__     Freeciv21 is free software: you can redistribute it
 \___  \____/   __/    and/or modify it under the terms of the GNU  General
     \_       _/          Public License  as published by the Free Software
       | @ @  \_               Foundation, either version 3 of the  License,
       |                              or (at your option) any later version.
     _/     /\                  You should have received  a copy of the GNU
    /o)  (o/\ \_                General Public License along with Freeciv21.
    \_____/ /                     If not, see https://www.gnu.org/licenses/.
      \____/        ********************************************************/

#include <cstdarg>
#include <cstring>

#include <ctime>

// utility
#include "fcintl.h"
#include "log.h"
#include "shared.h"
#include "support.h"

// common
#include "chat.h"
#include "game.h"
#include "packets.h"
#include "player.h"

// server
#include "console.h"
#include "notify.h"
#include "stdinhand.h"

#include "handchat.h"

#define MAX_LEN_CHAT_NAME                                                   \
  (2 * MAX_LEN_NAME + 10) // for form_chat_name() names

static void send_chat_msg(struct connection *pconn,
                          const struct connection *sender,
                          const struct ft_color color, const char *format,
                          ...) fc__attribute((__format__(__printf__, 4, 5)));

/**
   Returns whether 'dest' is ignoring the 'sender' connection.
 */
static inline bool conn_is_ignored(const struct connection *sender,
                                   const struct connection *dest)
{
  if (nullptr != sender && nullptr != dest) {
    return conn_pattern_list_match(dest->server.ignore_list, sender);
  } else {
    return false;
  }
}

/**
   Formulate a name for this connection, prefering the player name when
   available and unambiguous (since this is the "standard" case), else
   use the username.
 */
static void form_chat_name(struct connection *pconn, char *buffer,
                           size_t len)
{
  struct player *pplayer = pconn->playing;

  if (!pplayer || pconn->observer
      || strcmp(player_name(pplayer), ANON_PLAYER_NAME) == 0) {
    fc_snprintf(buffer, len, "(%s)", pconn->username);
  } else {
    fc_snprintf(buffer, len, "%s", player_name(pplayer));
  }
}

/**
   Send a chat message packet.
 */
static void send_chat_msg(struct connection *pconn,
                          const struct connection *sender,
                          const struct ft_color color, const char *format,
                          ...)
{
  struct packet_chat_msg packet;
  va_list args;

  va_start(args, format);
  vpackage_chat_msg(&packet, sender, color, format, args);
  va_end(args);

  send_packet_chat_msg(pconn, &packet);
}

static struct ft_color get_ftc_for_player(const char &c)
{
  switch (c) {
  case 'p':
  case 'P':
    return ftc_chat_player_purple;
  case 'b':
  case 'B':
    return ftc_chat_player_blue;
  case 'g':
  case 'G':
    return ftc_chat_player_green;
  case 'y':
  case 'Y':
    return ftc_chat_player_yellow;
  default:
    return ftc_chat_player_purple;
  }
}

/**
   Send private message to single connection.
 */
static void chat_msg_to_conn(struct connection *sender,
                             struct connection *dest, char *msg)
{
  char sender_name[MAX_LEN_CHAT_NAME], dest_name[MAX_LEN_CHAT_NAME];

  form_chat_name(dest, dest_name, sizeof(dest_name));

  if (conn_is_ignored(sender, dest)) {
    send_chat_msg(sender, nullptr, ftc_warning,
                  _("You cannot send messages to %s; you are ignored."),
                  dest_name);
    return;
  }

  msg = skip_leading_spaces(msg);
  form_chat_name(sender, sender_name, sizeof(sender_name));

  send_chat_msg(sender, sender, ftc_chat_private, "->*%s* %s", dest_name,
                msg);

  if (sender != dest) {
    send_chat_msg(dest, sender, ftc_chat_private, "*%s* %s", sender_name,
                  msg);
  }
}

/**
   Send private message to multi-connected player.
 */
static void chat_msg_to_player(struct connection *sender, char *msg)
{
  struct packet_chat_msg packet_self, packet_other;
  char sender_name[MAX_LEN_CHAT_NAME];
  struct connection *dest = nullptr; // The 'pdest' user.

  if(strlen(msg) <= 3 && msg[2] != ':') return;
  char player_char_to = msg[1];
  char player_char_from = sender->username[0];

  struct player *pdest = player_by_user_char(player_char_to);
  if(!pdest) return;

  // Find the user of the player_char_to.
  conn_list_iterate(pdest->connections, pconn)
  {
    if (!pconn->observer) {
      // Found it!
      if (conn_is_ignored(sender, pconn)) {
        send_chat_msg(sender, nullptr, ftc_warning,
                      _("You cannot send messages to %s; you are ignored."),
                      player_name(pdest));
        return; // NB: stop here, don't send to observers.
      }
      dest = pconn;
      break;
    }
  }
  conn_list_iterate_end;

  msg = msg + 3;
  msg = skip_leading_spaces(msg);
  form_chat_name(sender, sender_name, sizeof(sender_name));

  struct event_cache_players *players =
      event_cache_player_add(nullptr, pdest);
  players = event_cache_player_add(players, sender->playing);
  players = event_cache_player_add(players, pdest);
  package_chat_msg(&packet_self, sender, get_ftc_for_player(sender_name[0]),
                   "%c%c:{%s} %s", CHAT_PRIVATE_PREFIX, player_char_to,
                   sender_name, msg);
  event_cache_add_for_players(&packet_self, players);
  players = event_cache_player_add(nullptr, pdest);
  players = event_cache_player_add(players, sender->playing);
  players = event_cache_player_add(players, pdest);
  package_chat_msg(&packet_other, sender, get_ftc_for_player(sender_name[0]),
                   "%c%c:{%s} %s", CHAT_PRIVATE_PREFIX, player_char_from,
                   sender_name, msg);
  event_cache_add_for_players(&packet_other, players);
  con_write(C_COMMENT, "%s", packet_other.message);

  // Repeat the message for the sender.
  send_chat_msg(sender, sender, get_ftc_for_player(sender_name[0]), "%c%c:{%s} %s",
                CHAT_PRIVATE_PREFIX, player_char_to,
                sender_name, msg);

  // Send the message to destination.
  if (nullptr != dest && dest != sender) {
    send_chat_msg(dest, sender, get_ftc_for_player(sender_name[0]), "%c%c:{%s} %s",
                  CHAT_PRIVATE_PREFIX, player_char_from,
                  sender_name, msg);
  }
}

/**
   Send private message to all connections.
 */
static void chat_msg_to_all(struct connection *sender, char *msg)
{
  struct packet_chat_msg packet;
  char sender_name[MAX_LEN_CHAT_NAME];
  if(msg[0] != '\0') msg++;

  msg = skip_leading_spaces(msg);
  form_chat_name(sender, sender_name, sizeof(sender_name));

  package_chat_msg(&packet, sender, get_ftc_for_player(sender_name[0]), "%c<%s> %s",
                   CHAT_GLOBAL_PREFIX, sender_name, msg);
  con_write(C_COMMENT, "%s", packet.message);
  lsend_packet_chat_msg(game.est_connections, &packet);

  // Add to the event cache.
  event_cache_add_for_all(&packet);
}

/**
   Send private message to audit.
 */
static void chat_msg_to_audit(struct connection *sender, char *msg)
{
  struct packet_chat_msg packet;
  char sender_name[MAX_LEN_CHAT_NAME];
  if (msg[0] != '\0')
    msg++;

  QString q_str(msg);
  int colon_idx = q_str.indexOf(":");
  int sabotage_id = q_str.left(q_str.indexOf(':')).toInt();
  msg += colon_idx + 1;

  msg = skip_leading_spaces(msg);
  form_chat_name(sender, sender_name, sizeof(sender_name));

  package_chat_msg(&packet, sender, get_ftc_for_player(sender_name[0]),
                   "%c%d:<%s> %s", CHAT_AUDIT_PREFIX, sabotage_id, sender_name, msg);
  con_write(C_COMMENT, "%s", packet.message);
  lsend_packet_chat_msg(game.est_connections, &packet);

  // Add to the event cache.
  event_cache_add_for_all(&packet);
}

/**
   Handle a chat message packet from client:
   1. Work out whether it is a server command and if so run it;
   2. Otherwise work out whether it is directed to a single player, or
      to a single connection, and send there.  (For a player, send to
      all clients connected as that player, in multi-connect case);
   3. Or it may be intended for all allied players.
   4. Else send to all connections (game.est_connections).

   In case 2, there can sometimes be ambiguity between player and
   connection names.  By default this tries to match player name first,
   and only if that fails tries to match connection name.  User can
   override this and specify connection only by using two colons ("::")
   after the destination name/prefix, instead of one.

   The message sent will name the sender, and via format differences
   also indicates whether the recipient is either all connections, a
   single connection, or multiple connections to a single player.

   Message is also echoed back to sender (with different format),
   avoiding sending both original and echo if sender is in destination
   set.
 */
void handle_chat_msg_req(struct connection *pconn, const char *message)
{
  char real_message[MAX_LEN_MSG], *cp;
  bool double_colon;

  sz_strlcpy(real_message, message);

  log_warning("Raw msg: %s", real_message);

  /* This loop to prevent players from sending multiple lines which can
   * be abused */
  for (cp = real_message; *cp != '\0'; cp++) {
    if (*cp == '\n' || *cp == '\r') {
      *cp = '\0';
      break;
    }
  }

  /* Server commands are prefixed with '/', which is an obvious
     but confusing choice: even before this feature existed,
     novice players were trying /who, /nick etc.
     So consider this an incentive for IRC support,
     or change it in chat.h - rp
  */
 switch(real_message[0]) {
    case SERVER_COMMAND_PREFIX:
      // pass it to the command parser, which will chop the prefix off
      (void) handle_stdin_input(pconn, real_message);
      return;
    case CHAT_GLOBAL_PREFIX:
      // Send to all command
      chat_msg_to_all(pconn, real_message);
      return;
    case CHAT_PRIVATE_PREFIX:
      // Send to player command
      chat_msg_to_player(pconn, real_message);
      return;
    case CHAT_AUDIT_PREFIX:
      // Send to audit command
      chat_msg_to_audit(pconn, real_message);
      return;
    default:
      // Messages must have prefix.
      log_warning("Message without prefix: %s", real_message);
      return;
  }

  /* Want to allow private messages with "player_name: message",
     (or "connection_name: message"), including unambiguously
     abbreviated player/connection name, but also want to allow
     sensible use of ':' within messages, and _also_ want to
     notice intended private messages with (eg) mis-spelt name.

     Approach:

     If there is no ':', or ':' is first on line,
          message is global (send to all players)
     else if the ':' is double, try matching part before "::" against
          connection names: for single match send to that connection,
          for multiple matches complain, else goto heuristics below.
     else try matching part before (single) ':' against player names:
          for single match send to that player, for multiple matches
          complain
     else try matching against connection names: for single match send
          to that connection, for multiple matches complain
     else if some heuristics apply (a space anywhere before first ':')
          then treat as global message,
     else complain (might be a typo-ed intended private message)
  */

  /*cp = strchr(real_message, CHAT_DIRECT_PREFIX);

  if (cp && (cp != &real_message[0])) {
    enum m_pre_result match_result_player, match_result_conn;
    struct player *pdest = nullptr;
    struct connection *conn_dest = nullptr;
    char name[MAX_LEN_NAME];
    char *cpblank;

    (void) fc_strlcpy(name, real_message,
                      MIN(sizeof(name), cp - real_message + 1));

    double_colon = (*(cp + 1) == CHAT_DIRECT_PREFIX);
    if (double_colon) {
      conn_dest = conn_by_user_prefix(name, &match_result_conn);
      if (match_result_conn == M_PRE_AMBIGUOUS) {
        complain_ambiguous(pconn, name, 1);
        return;
      }
      if (conn_dest && match_result_conn < M_PRE_AMBIGUOUS) {
        chat_msg_to_conn(pconn, conn_dest, cp + 2);
        return;
      }
    } else {
      // single colon
      pdest = player_by_name_prefix(name, &match_result_player);
      if (match_result_player == M_PRE_AMBIGUOUS) {
        complain_ambiguous(pconn, name, 0);
        return;
      }
      if (pdest && strcmp(player_name(pdest), ANON_PLAYER_NAME) == 0) {
        complain_ambiguous(pconn, name, 2);
        return;
      }
      if (pdest && match_result_player < M_PRE_AMBIGUOUS) {
        chat_msg_to_player(pconn, pdest, cp + 1);
        return;
        // else try for connection name match before complaining
      }
      conn_dest = conn_by_user_prefix(name, &match_result_conn);
      if (match_result_conn == M_PRE_AMBIGUOUS) {
        complain_ambiguous(pconn, name, 1);
        return;
      }
      if (conn_dest && match_result_conn < M_PRE_AMBIGUOUS) {
        chat_msg_to_conn(pconn, conn_dest, cp + 1);
        return;
      }
    }
    cpblank = strchr(real_message, ' ');
    if (!cpblank || (cp < cpblank)) {
      if (double_colon) {
        notify_conn(pconn->self, nullptr, E_CHAT_ERROR, ftc_server,
                    _("There is no connection by the name %s."), name);
      } else {
        notify_conn(pconn->self, nullptr, E_CHAT_ERROR, ftc_server,
                    _("There is no player nor connection by the name %s."),
                    name);
      }
      return;
    }
  }
  // global message:
  chat_msg_to_all(pconn, real_message);
  */
}
