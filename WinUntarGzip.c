/*
 * winuntargzip.c
 *
 *  Created on: May 21, 2020
 *      Author: Caleb
 */
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

struct gzip_member {
	unsigned char id1;
	unsigned char id2;
	unsigned char cm;
	unsigned char flag;
	unsigned int mtime;
	unsigned char xfl;
	unsigned char os;
};

void dump_gzip_struct(struct gzip_member *s) {
	printf("Gzip Struct Dump:\n");
	printf("\tid1: 0x%x\n", s->id1);
	printf("\tid2: 0x%x\n", s->id2);
	printf("\tcm: 0x%x\n", s->cm);
	printf("\tflag: 0x%x\n", s->flag);
	printf("\tmtime: 0x%x\n", s->mtime);
	printf("\txfl: 0x%x\n", s->xfl);
	printf("\tos: 0x%x\n", s->os);
}


int main(int argc, char *argv[]) {

	if(argc < 3) {
		printf("Usage: winuntg <tar.gz file> <destination>");
	}

	FILE *tar_file = fopen(argv[1], "r");
	int tar_fd = fileno(tar_file);
	if(tar_file == NULL) {
		printf("Could not parse tar file");
		exit(1);
	}

	DIR* dir = opendir(argv[2]);
	if(dir) {
//		printf("Directory already exists");
//		closedir(dir);
//		exit(1);
	}
	else if(ENOENT == errno) {
		// Make the directory
		if(mkdir(argv[2]) == -1) {
			printf("Could not create destination directory");
			exit(1);
		}
	} else {
		printf("Could not access destination directory");
		exit(1);
	}

	int size = sizeof(struct gzip_member);
	unsigned char buf[size];
	int read_sz = read(tar_fd, buf, size);
	if(read_sz != size) {
		printf("Unrecognized file format");
		exit(1);
	}
	struct gzip_member *header = calloc(1, sizeof(struct gzip_member));
	memcpy(header, buf, size);
	dump_gzip_struct(header);


	return 0;
}
