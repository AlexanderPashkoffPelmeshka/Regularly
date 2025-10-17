#pragma once

class State {
  inline static u_int32_t numerous = 0;

  std::unordered_map<char, std::vector<State*>> links;
  u_int32_t id;
  bool is_finished = false;

 public:

  State() : id(numerous++) {};

  void add_link(char c, State* another) {
    links[c].push_back(another);
  }

  bool is_finish() const noexcept { return is_finished; }
  void set_finish(bool val) noexcept { is_finished = val; }
  u_int32_t get_id() const noexcept { return id; }
  const auto& get_links() const noexcept { return links; }
};

struct Tree {
  std::vector<State*> buffer;
  std::vector<size_t> ends;
  size_t start;

  Tree(const Tree&) = delete;
  Tree& operator=(const Tree&) = delete;

  Tree(Tree&& another) noexcept : 
    buffer(std::move(another.buffer)), ends(std::move(another.ends)), 
    start(another.start) {
    another.start;
  }

  Tree() {};

  void print_Tree() {
    std::cout << std::format("Начальная вершина {}\n", buffer[start]->get_id());
    std::cout << "Конечная вершины: ";
    for (const auto& x : ends) std::cout << buffer[x]->get_id() << "  ";
    std::cout << "\n";
    for (const auto& x : buffer) {
      for (const auto& [key, vector] : x->get_links()) {
        for (const auto& y : vector) {
          std::cout << std::format("({}) --{}--> ({})\n", x->get_id(), key, y->get_id());
        }
      }
    }
  }

  ~Tree() {
    for (auto&& it : buffer) {
      delete it;
    }
  }
};

class NKA_Builder {
  using NKA = std::pair<State*, State*>;
  
  std::vector<char> alphabets;

 public:
  NKA_Builder(std::vector<char>&& another) : alphabets(std::move(another)) {};
  NKA_Builder(const std::vector<char>& another) : alphabets(another) {};

  Tree build(const std::string& polish_line) {
    std::stack<NKA> stack;
    Tree tree;

    for (char c : polish_line) {
      if (std::find(alphabets.begin(), alphabets.end(), c) != alphabets.end()) {
        State* start = new State();
        State* end = new State();
        end->set_finish(true);
        start->add_link(c, end);

        stack.push({start, end});
        tree.buffer.push_back(start);
        tree.buffer.push_back(end);
      } else if (c == '1') {
        State* start = new State();
        State* end = new State();
        end->set_finish(true);
        start->add_link('\0', end);

        stack.push({start, end});
        tree.buffer.push_back(start);
        tree.buffer.push_back(end);
      } else if (c == '+') {
        if (stack.size() < 2) {
          tree.~Tree();
          throw std::invalid_argument("Не достаточно операндов");
        }

        auto [start2, end2] = stack.top();
        stack.pop();

        auto [start1, end1] = stack.top();
        stack.pop();

        State* new_start = new State();
        State* new_end = new State();
        new_end->set_finish(true);

        new_start->add_link('\0', start1);
        new_start->add_link('\0', start2);
        end1->add_link('\0', new_end);
        end2->add_link('\0', new_end);
        end1->set_finish(false);
        end2->set_finish(false);

        stack.push({new_start, new_end});
        tree.buffer.push_back(new_start);
        tree.buffer.push_back(new_end);
      }
      else if (c == '.') {
        if (stack.size() < 2) {
          tree.~Tree();
          throw std::invalid_argument("Не достаточно операндов");
        }

        auto [start2, end2] = stack.top();
        stack.pop();

        auto [start1, end1] = stack.top();
        stack.pop();

        end1->add_link('\0', start2);
        end1->set_finish(false);

        stack.push({start1, end2});
      }
      else if (c == '*') {
        if (stack.empty()) {
          tree.~Tree();
          throw std::invalid_argument("Не достаточно операндов");
        }

        auto [start_old, end_old] = stack.top(); stack.pop();

        State* new_start = new State();
        State* new_end = new State();
        new_end->set_finish(true);

        new_start->add_link('\0', new_end);
        new_start->add_link('\0', start_old);

        end_old->add_link('\0', new_end);
        end_old->add_link('\0', start_old);
        end_old->set_finish(false);

        stack.push({new_start, new_end});
        tree.buffer.push_back(new_start);
        tree.buffer.push_back(new_end);
      } else {
        tree.~Tree();
        throw std::invalid_argument(std::format("Неизвестный символ: {}\n", std::string(1, c)));
      }
    }

    auto [final_start, final_end] = stack.top();
    tree.start = final_start->get_id();

    for (State* state : tree.buffer) {
      if (state->is_finish()) {
        tree.ends.push_back(state->get_id());
      }
    } 
    return tree;
  }
};
