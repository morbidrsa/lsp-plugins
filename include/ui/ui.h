/*
 * ui.h
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_UI_H_
#define _UI_UI_H_

#define LSP_UI_SIDE

#include <core/types.h>
#include <core/status.h>

#include <stddef.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

// Windowing system
#include <ui/ws/ws.h>

// Toolkit system
#include <ui/tk/tk.h>

// Control system
#include <ui/ctl/ctl.h>

namespace lsp
{
    class ui_builder;

    using namespace ::lsp::ws;
    using namespace ::lsp::tk;
    using namespace ::lsp::ctl;
}

// Misc data structures
#include <data/cvector.h>

// Core
#include <core/types.h>
#include <core/debug.h>

// Metadata
#include <metadata/metadata.h>

// Configuration parsing
#include <ui/XMLHandler.h>
#include <ui/XMLParser.h>

// Common definitions
#include <ui/common.h>
#include <ui/IUIWrapper.h>

// Plugin UI
#include <ui/plugin_ui.h>
#include <ui/ui_builder.h>

#endif /* _UI_UI_H_ */
