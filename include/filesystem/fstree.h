fstree_t new_fstree ( void );
error add_child ( fstree_node_t , fstree_node_t );
error add_directory ( fstree_node_t , fstree_node_t );
fstree_node_t new_fstree_node (bool , string , unsigned);
fstree_t branch_tree ( fstree_t );
error update_child_from_path(fstree_t, string, bool);
fstree_node_t find_child_by_path(fstree_node_t, char*);
fstree_node_t get_node_from_path(fstree_t,string);
