#pragma once

void killed_optimisation(const DFA& object) {
  for (auto&& x : object.get_states()) {
    if (x->is_closed()) {
      x->set_kill_optimisation(true);
      break;
    }
  }
}