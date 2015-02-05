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
//switch to convert to fixed width
int cfw = 0;
//switch to write to file
int wf = 0;
//file path to wrie to
char fptowrt[PATH_MAX+1];

void * convert_to_fixed_width(char * csv){
// Find how many collumns are in the file
  int cnum = 0;
  int i = 0;
  char c = csv[i];
  while((c != '\0') && (c != '\n')){
    c = csv[i++];
    //We test all three in case there is only one row of records
    if(c == ',' || c =='\n' || c == '\0'){
      cnum++;
    }
    if(c == '\n'){
      c = csv[i++];
    }
  }

  int num_columns = cnum;
  // Create array of ints to hold how long each collums max length is
  // Add a space after each collumn to make it human readable
  int clength[cnum];
  for(int i = cnum; i >= 0; i--){
    clength[i] = 0;
  }
  
  int col = 0;
  int rows = 1;
  cnum = 0;
  i = 0;
  c = csv[i];
  while(c != '\0'){
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
      rows++;
    }
    col++;
    c = csv[i++];
  }

  // reset all counters
  i = 0;
  c = csv[i];
  cnum = 0;
  col = 0;


  // find complete size of fixed width file and create it
  int fw_file_size = 0;
  for(int i = num_columns; i >= 0; i--){
    fw_file_size += clength[i];
  }
  fw_file_size = fw_file_size * rows;

  char * fw = (char *)malloc(sizeof(char)*fw_file_size+1); // The fixed width file to return 
  int fw_pos = 0;
  while(c != '\0'){
    while((c != ',') && (c != '\0') && (c != '\n')){
      fw[fw_pos] = c;
      fw_pos++;
      col++;
      c = csv[i++];
    }
    while(col < clength[cnum]){
      fw[fw_pos] =  ' ';
      fw_pos++;
      col++;
    }
    cnum++;
    col = 0;
    
    if(c == '\n'){
      fw[fw_pos] =  '\n';
      fw_pos++;
      cnum = 0;
    }
    c = csv[i++];
  }
  fw[fw_pos] = '\0';

  return fw;
}

int main(int argc, char *argv[]){
  int opt;
  char path[PATH_MAX + 1];

  if(argc < 2){
    // If no options have been specified print help text
      fprintf(stdout, "usage: fixedwidth2csv <options>\n");
      fprintf(stdout, "-f <path to file to convert>\n");
      fprintf(stdout, "-w <path of file to write to>\n");
      fprintf(stdout, "If -w is not specified will output to stdout\n");
      return -1;
  }
  
  while((opt = getopt(argc, argv, "ocw:f:")) != -1) {
    switch(opt) {
    case 'c':
      // We are converting to fixed width
      cfw = 1;
      break;
    case 'w':
      // path of file to write to
      realpath(optarg, fptowrt);
      wf = 1;
      break;
    case 'f':
      //path of file to convert
      realpath(optarg, path);
      
      if((file = fopen(path, "r")) == NULL){
	fprintf(stdout, "Failed to open file\n");
      }
      
      break;
      //getopt handles unknown options so there is no default case
    }
  }
  // We should have a file to edit by now but we check anyway
  if(file == NULL){
    return -1;
  }

  // We now want the file in a array so we can move back and forth through it
  struct stat sb;
  stat(path, &sb);
  char array[sb.st_size];
  int array_size = 0;
  int i = 0;
  int c = 0;
  while((c = fgetc(file)) != EOF){
    array[i] = c;
    i++;
  }
  array[i++] = '\0';

  char * fw = convert_to_fixed_width(array);
  if(wf){
    FILE * file_to_write = fopen(fptowrt, "w+");
    fputs(fw, file_to_write);
    
  } else {
    fprintf(stdout, "%s", fw);
    fprintf(stdout, "\n"); // add a newline before exiting
    }
  free(fw);
  return 0;
}
