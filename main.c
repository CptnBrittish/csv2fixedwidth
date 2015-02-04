// This program converts between fixed width files and csv files

#include <stdio.h>

#define _GNU_SOURCE
#include <stdlib.h>
#include <linux/limits.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>

FILE *file;

int main(int argc, char *argv[]){
  int opt;
  char path[PATH_MAX + 1];
  
  if(argc < 2){
    printf("No input\n");
    return -1;
  }
  while((opt = getopt(argc, argv, "f:")) != -1) {
    switch(opt) {
    case 'f':
      realpath(optarg, path);
      
      if((file = fopen(path, "r")) == NULL){
	fprintf(stdout, "Failed to open file\n");
      }
      
      break;
    default:
      fprintf(stdout, "No option found\n");
      break;
    }
  }
  // We should have a file to edit by now

  // We now want the file in a array so we can move back and forth through it
  struct stat sb;
  stat(path, &sb);
  int array[sb.st_size];
  int array_size = 0;
  int i = 0;
  int c = 0;
  while((c = fgetc(file)) != EOF){
    array[i] = c;
    i++;
    array_size++;
  }
  array[i++] = EOF;
  array_size++;

  // Find how many collumns are in the file
  int cnum = 0;
  i = 0;
  c = array[i];
  while((c != EOF) && (c != '\n')){
    c = array[i++];
    //We test all three in case there is only one row of records
    if(c == ',' || c =='\n' || c == EOF){
      cnum++;
    }

  }

  // Create array of ints to hold how long each collums max length is
  // Add a space after each collumn to make it human readable
  int clength[cnum];
  for(int i = cnum; i >= 0; i--){
    clength[i] = 0;
  }
  int col = 0;
  cnum = 0;
  i = 0;
  c = array[i];
  while(i < array_size){
    if(c == ',' || c =='\n'){
      if(col+1 > clength[cnum]){
	clength[cnum] = col;
      }
      col = 0;
      cnum++;
    }
    if(c == '\n'){
      cnum = 0;
      col = 0;
    }
    col++;
    c = array[i++];
  }

  i = 0;
  c = array[i];
  cnum = 0;
  col = 0;
  for(int i = 0; i <= 2; i++){
  }
  while(i < array_size){
    while((c != ',') && (i < array_size) && (c != '\n')){
      fprintf(stdout, "%c", c);
      col++;
      c = array[i++];
    }
    while(col < clength[cnum]){
      fprintf(stdout, " ");
      col++;
    }
    cnum++;
    col = 0;
    
    if(c == '\n'){
      fprintf(stdout, "\n");
      cnum = 0;
    }
    c = array[i++];
  }
  fprintf(stdout, "\n");  
  return 0;
}
