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
#include <sys/types.h>
#include <unistd.h>

struct __attribute__((__packed__)) gzip_member {
	unsigned char id1;
	unsigned char id2;
	unsigned char cm;
	unsigned char flag;
	unsigned int mtime;
	unsigned char xfl;
	unsigned char os;
	/*unsigned int crc32;
	unsigned int isize;*/
};

struct deflate_member {
	unsigned char b1;
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
/*	printf("\tcrc32: 0x%x\n", s->crc32);
	printf("\tisize: 0x%x\n", s->isize);*/
}

void dump_deflate_struct(struct deflate_member *s) {
	printf("Deflate Struct Dump:\n");
	printf("\tbfinal: 0x%x\n", s->b1);
}

int check_gzip_header(struct gzip_member *s) {
	unsigned int MAGIC_ID1 = 0X1F;
	unsigned int MAGIC_ID2 = 0x8B;
	unsigned int DEFLATE_CM = 0x8;
	dump_gzip_struct(s);
	if(s->id1 != MAGIC_ID1 | s->id2 != MAGIC_ID2) {
		return -1;
	}
	if(s->cm != DEFLATE_CM) {
		return -1;
	}
//	if(s->flag != 0x0) {
//		return -1;
//	}

	return 0;
}


int main(int argc, char **argv) {

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
	printf("Sizeof gzip header: %d\n", sizeof(unsigned int));
	unsigned char buf[size];
	memset(buf, 0x0, sizeof buf);
	ssize_t read_sz = read(tar_fd, buf, size);
	if(read_sz != size) {
		printf("Unrecognized file format");
		exit(1);
	}
	struct gzip_member *header = calloc(1, sizeof(struct gzip_member));
	memcpy(header, buf, size);
	if(check_gzip_header(header) == -1) {
		printf("Unrecognized gzip header");
		exit(1);
	}
	// Skip comment
	char fn[256];
	memset(fn, 0x0, sizeof fn);
	int idx = 0;
	unsigned char byte = 0xFF;
	while(byte != 0x0) {
		read(tar_fd, &byte, 1);
		fn[idx++] = byte;
	}
	printf("Got filename: %s\n", fn);
	memset(buf, 0x0, sizeof buf);
	size = sizeof(struct deflate_member);
	printf("File offest: %d\n", ftell(tar_file));
	read_sz = read(tar_fd, buf, size);
	if(read_sz != size) {
		printf("Read size: %d | Wanted size: %d | Sizeof deflate_member: %d\n", read_sz, size, sizeof(struct deflate_member));
		printf("Unrecognized file format, deflate\n");
		printf("Data in buffer: 0x%x", buf[0]);
		exit(1);
	}
	struct deflate_member *dm = calloc(1, sizeof(struct deflate_member));
	memcpy(dm, buf, size);
	dump_deflate_struct(dm);


	return 0;
}
