fstree_t new_fstree ( void );
error add_child ( fstree_node_t , fstree_node_t );
error add_directory ( fstree_node_t , fstree_node_t );
fstree_node_t new_fstree_node ( bool , string , unsigned );
fstree_t branch_tree ( fstree_t );
