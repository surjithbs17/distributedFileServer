#!/bin/bash

SERVER_FILE_NAME="dfs.c"
#print variable on a screen
make all

gnome-terminal --command="./dfs dfs1 10001"
gnome-terminal --command="./dfs dfs2 10002"
gnome-terminal --command="./dfs dfs3 10003"
gnome-terminal --command="./dfs dfs4 10004"