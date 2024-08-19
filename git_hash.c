#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <sys/stat.h>
#include <string.h>

unsigned int simple_hash(const unsigned char *data, size_t len) {
	unsigned int hash = 0;
	for (size_t i = 0; i < len; ++i) {
		hash = hash * 31 + data[i];  // 31 是一个常用的哈希因子
	}
	return hash;
}

void init() {
	mkdir(".git", 0755);
	mkdir(".git/objects", 0755);
	mkdir(".git/refs", 0755);
	FILE *head = fopen(".git/HEAD", "w");
	fprintf(head, "refs/heads/master");
	fclose(head);
	printf("Initialized empty Git repository.\n");
}

void hash_object(const char *file_path) {
	FILE *file = fopen(file_path, "rb");
	if (!file) {
		printf("Error opening file.\n");
		return;
	}

	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	unsigned char *buffer = malloc(file_size);
	fread(buffer, 1, file_size, file);

	unsigned int hash = simple_hash(buffer, file_size);

	char object_path[128];
	sprintf(object_path, ".git/objects/%08x", hash); // 以8位十六进制表示

	FILE *out = fopen(object_path, "wb");
	fwrite(buffer, 1, file_size, out);

	printf("Saved object with hash: %08x\n", hash);

	fclose(file);
	fclose(out);
	free(buffer);
}

void commit(const char *message) {
	// 在这里，你可以将工作区的文件哈希后保存起来，并记录提交信息。
	// 为简单起见，这里只对一个文件进行哈希处理。
	hash_object("example.txt");
	// 在 `.git/refs/heads/master` 文件中记录提交的 hash
	FILE *head = fopen(".git/refs/heads/master", "w");
	fprintf(head, "%s\n", "commit_hash_placeholder"); // 这里应该是实际计算得到的 hash
	fclose(head);
}

void log_history() {
	FILE *file = fopen(".git/refs/heads/master", "r");
	if (!file) {
		printf("Error opening log file.\n");
		return;
	}

	char hash[65];
	while (fscanf(file, "%64s", hash) != EOF) {
		printf("Commit: %s\n", hash);
	}

	fclose(file);
}

void checkout(const char *commit_hash) {
	char object_path[128];
	sprintf(object_path, ".git/objects/%s", commit_hash);

	FILE *file = fopen(object_path, "rb");
	if (!file) {
		printf("Commit not found: %s\n", commit_hash);
		return;
	}

	// 在这里恢复文件内容。
	fclose(file);
}



int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Usage: %s <command>\n", argv[0]);
		return 1;
	}

	if (strcmp(argv[1], "init") == 0) {
		init();
	} else if (strcmp(argv[1], "commit") == 0) {
		if (argc < 3) {
			printf("Usage: %s commit <message>\n", argv[0]);
			return 1;
		}
		commit(argv[2]);
	} else {
		printf("Unknown command: %s\n", argv[1]);
	}

	return 0;
}
