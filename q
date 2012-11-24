diff --git a/bin/cvsclient b/bin/cvsclient
deleted file mode 100755
index b7669d5..0000000
Binary files a/bin/cvsclient and /dev/null differ
diff --git a/bin/cvsserver b/bin/cvsserver
deleted file mode 100755
index fe433ea..0000000
Binary files a/bin/cvsserver and /dev/null differ
diff --git a/src/cvs/a.out b/src/cvs/a.out
deleted file mode 100755
index 592c3d6..0000000
Binary files a/src/cvs/a.out and /dev/null differ
diff --git a/src/cvs/functions.c b/src/cvs/functions.c
index 83ca845..b0878a8 100644
--- a/src/cvs/functions.c
+++ b/src/cvs/functions.c
@@ -222,6 +222,8 @@ int commit(char * path, int client_id) {
 	fstree_t client_tree = get_client_tree(base_client_id);
 	fstree_node_t current_node = client_tree->root;
 	commit_recursive(current_node, path, REPOSITORY_PATH);
+	repository_tree = new_fstree();
+	repository_tree = retrieve_tree(REPOSITORY_PATH, repository_tree->root);
 }
 
 int diff(char * path, int client_id) {
diff --git a/src/filesystem/filesystem.c b/src/filesystem/filesystem.c
index 1a784b0..826922b 100644
--- a/src/filesystem/filesystem.c
+++ b/src/filesystem/filesystem.c
@@ -10,7 +10,6 @@ void retrieve_tree(char * path , fstree_node_t node);
 fstree_t filesystem_init();
 
 fstree_t filesystem_init() {
-	
 	 fstree_t tree = new_fstree();
 	 retrieve_tree(REPOSITORY_PATH, tree->root);
 	 return tree;
