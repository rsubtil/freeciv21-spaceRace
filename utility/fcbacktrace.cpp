/***********************************************************************
 Freeciv - Copyright (C) 1996 - A Kjeldberg, L Gregersen, P Unold
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
***********************************************************************/

#ifdef HAVE_CONFIG_H
#include <fc_config.h>
#endif /* HAVE_CONFIG_H */

#include <QLoggingCategory>
#include <sstream>

#include <backward.hpp>

/* utility */
#include "log.h"
#include "shared.h"

#include "fcbacktrace.h"

/* We don't want backtrace-spam to testmatic logs */
#if defined(FREECIV_DEBUG) && !defined(FREECIV_TESTMATIC)
#define BACKTRACE_ACTIVE 1
#endif

#ifdef BACKTRACE_ACTIVE
#define MAX_NUM_FRAMES 64

Q_LOGGING_CATEGORY(stack_category, "freeciv.stacktrace")

namespace {
static QtMessageHandler previous = nullptr;

static void backtrace_log(QtMsgType type, const QMessageLogContext &context,
                          const QString &message);
void backtrace_print(QtMsgType type, const QMessageLogContext &context);
} // anonymous namespace
#endif /* BACKTRACE_ACTIVE */

/********************************************************************/ /**
   Take backtrace log callback to use
 ************************************************************************/
void backtrace_init(void)
{
#ifdef BACKTRACE_ACTIVE
  previous = qInstallMessageHandler(backtrace_log);
#endif
}

/********************************************************************/ /**
   Remove backtrace log callback from use
 ************************************************************************/
void backtrace_deinit(void)
{
#ifdef BACKTRACE_ACTIVE
  qInstallMessageHandler(previous);
#endif /* BACKTRACE_ACTIVE */
}

#ifdef BACKTRACE_ACTIVE
/********************************************************************/ /**
   Main backtrace callback called from logging code.
 ************************************************************************/
namespace {
static void backtrace_log(QtMsgType type, const QMessageLogContext &context,
                          const QString &message)
{
  if (type == QtFatalMsg || type == QtCriticalMsg) {
    backtrace_print(type, context);
  }

  if (previous != nullptr) {
    // Call chained callback after printing the trace, because it might
    // abort()
    previous(type, context, message);
  }
}
} // anonymous namespace

namespace {

/********************************************************************/ /**
   Print backtrace
 ************************************************************************/
void backtrace_print(QtMsgType type, const QMessageLogContext &context)
{
  if (!stack_category().isEnabled(QtDebugMsg)) {
    // We won't print anything anyway. Since walking the stack is
    // expensive, return immediately.
    return;
  }

  using namespace backward;
  StackTrace st;
  st.load_here(MAX_NUM_FRAMES);

  // Generate the trace string
  Printer p;
  p.object = false;
  p.address = true;

  std::stringstream ss;
  p.print(st, ss);

  // Create a new context with the correct category name.
  QMessageLogContext modified_context(context.file, context.line,
                                      context.function,
                                      stack_category().categoryName());

  // Print
  std::string line;
  while (std::getline(ss, line)) {
    // Do the formatting manually (this is called from the message handler
    // and automatic formatting doesn't appear to work there).
    qCDebug(stack_category).noquote()
        << qFormatLogMessage(type, modified_context, line.data());
  }
}

} // anonymous namespace
#endif /* BACKTRACE_ACTIVE */
