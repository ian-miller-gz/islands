// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <sys/types.h>

#include <string>

#include <cartridge/interface.hpp>
#include <island/graphics/backend/passes.hpp>
#include <island/graphics/color.hpp>
#include <island/gui/gui.hpp>
#include <islands/select.hpp>
#include <network.hpp>
#include <shell/os/os.hpp>

namespace LAUNCHER {

struct Entry {
  String name;
  String summary;
  String directory;
  Flag bundle = false;
  Flag built = false;
  String author;
  String release;
  String updated;
  Whole depth = 0;
  Flag open = false;
  Flag last = false;
};

struct Control {
  String id;
  String label;
  String summary;
  String runs;
  Flag on = false;
  Flag ready = false;
};

struct Child {
  pid_t pid = 0;
  String name;
  String directory;
  Flag running = true;
  pid_t leader = 0;
};

constexpr STRING::Hot LAYOUT = "launcher.gui";
constexpr STRING::Hot HOME = "cartridges";
constexpr STRING::Hot MANIFEST = "manifest.yaml";

inline GUI::Handle document = GUI::NONE;
inline String showing;
inline String location = HOME;
inline Vector<Entry> entries;
inline String chosen;
inline Vector<Child> children;
inline Vector<Control> controls;

void scan();
void identify(Entry &entry);
void tick(Whole box);
auto list(const String &directory, Whole depth) -> Vector<Entry>;
void choose(Whole entry);
void open(Whole entry);
void toggle(Vector<Entry> &rows, Whole entry);
void toggle(Whole entry);
void visit(STRING::Hot area);
void enter(STRING::Hot call);
void start();
auto launch(const Entry &entry, pid_t leader) -> pid_t;
void reap();
void stop(pid_t pid);
void stop();
void dismiss(pid_t pid);
void update();
auto prompt(GFX::Viewport viewport) -> Flag;

namespace SETUP {
constexpr Whole BOXES = 6;
void read();
void refresh();
}  // namespace SETUP

namespace GET {
auto watched() -> Flag;
auto bare(const String &line) -> String;
}  // namespace GET
}  // namespace LAUNCHER

#include "state.ids.hpp"
#include "state.layout.hpp"
#include "state.watch.hpp"
#include "state.idle.hpp"
