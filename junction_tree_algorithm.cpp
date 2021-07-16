#include<iostream>
#include<vector>
#include<string>
#include<memory>


namespace JTA {
  
  enum class TREE_STATE
    {
     TREE_STATE_ZERO,
     TREE_STATE_COLLECTED,
     TREE_STATE_DISTRIBUTED
    };

  
  struct node {
    int node_id;
    double potential;
    bool is_root;
    std::vector<std::shared_ptr<node>> children;

    void absorb_from_child(std::shared_ptr<node> child) {
      this->potential = 2 * child->potential;
    }

    void absorb_from_parent(std::shared_ptr<node> parent) {
      this->potential = 0.5 * parent->potential;
    }
    
  };

  struct junction_tree {
    std::vector<std::shared_ptr<node>> nodes; 
    int root_id;
    TREE_STATE state = TREE_STATE::TREE_STATE_ZERO;
  };

  void collect(std::shared_ptr<node> node) {
    auto children = node->children;
    for (auto & child : children) {
      collect(child);
    
      // Absorption/update:
      std::cout <<
	"parent_id: " << node->node_id << " (value: " << node->potential << ") " << 
	"child_id: "  << child->node_id << " (value: " << child->potential << ") ";

      node->absorb_from_child(child);
      std::cout << "parent new vaule: " << node->potential << "\n";
      
    }
  }

  void collect_evidence(junction_tree& tree) {
    collect(tree.nodes[tree.root_id]);
    tree.state = TREE_STATE::TREE_STATE_COLLECTED;
  }

  void distribute(std::shared_ptr<node> node) {
    auto children = node->children;
    for (auto & child : children) {
    
      // Absorption/update:
      std::cout <<
	"parent_id: " << node->node_id << " (value: " << node->potential << ") " << 
	"child_id: "  << child->node_id << " (value: " << child->potential << ") ";

      child->absorb_from_parent(node);
      std::cout << "child new vaule: " << child->potential << "\n";
      
      distribute(child);
    }
  }

  void distribute_evidence(junction_tree& tree) {
    distribute(tree.nodes[tree.root_id]);
    tree.state = TREE_STATE::TREE_STATE_DISTRIBUTED;
  }

  
}

using jta_node = std::shared_ptr<JTA::node>;

int main() {

  /*
    The rooted junction tree with root = 1:

         4
         |
         v
    0--->1<--2<--3
         ^
         |
	 5
  */
  
  
  std::vector<std::vector<int>> children =
    {
     {},           // 0
     {0, 2, 4, 5}, // 1
     {3},          // 2
     {},           // 3
     {},           // 4
     {}            // 5
  };

  std::vector<double> pot_vals {5,6,4,2,1,3};
  
  int n_children = children.size();
  int root_node_id = 1;
  std::vector<jta_node> nodes;

  for (int i = 0; i < n_children; i++) {
    jta_node node_i(new JTA::node);
    node_i->potential = pot_vals[i];
    node_i->node_id = i;
    node_i->is_root = i == root_node_id ? true : false;
    nodes.push_back(node_i);
  }

  for (int i = 0; i < n_children; i++) {
    for (auto & e : children[i]) {
      nodes[i]->children.push_back(nodes[e]);
    }
  }

  // Check if the number of children is correct
  // ------------------------------------------
  // int k = 1;
  // for (auto & e : nodes) {
  //   std::cout << "k: " << k << "\n";
  //   k += 1;
  //   auto children = e->children;
  //   std::cout << children.size() << "\n";
  // }
  // ------------------------------------------
  
  JTA::junction_tree tree = {nodes, root_node_id};

  JTA::collect_evidence(tree);
  std::string state =  static_cast<int>(tree.state) == 1 ? "true" : "false";
  std::cout << "Collected: " <<  state << "\n";

  std::cout << "\n";
  
  JTA::distribute_evidence(tree);
  state = static_cast<int>(tree.state) == 2 ? "true" : "false";
  std::cout << "Distributed: " <<  state << "\n";
  
  return 0;
}
