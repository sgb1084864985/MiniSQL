#include "IndexFileHead.h"

//inline int IndexFileHead::getNewPage() {
//	if (alloc_pos == NULL_POS) throw IndexException(BLOCK_ALLOC_ERROR, "call addNewPage() first!");
//	return alloc_pos;
//}
//

int IndexFileHead::getNewPage() {
	if (free_pos == NULL_POS) return ++last_pos;
	else {
		Page free_block;
		int rslt = free_pos;
		free_block.tableName = p.tableName;
		free_block.pageType = IndexPage;
		free_block.ofs = free_pos;
		bm.readPage(free_block);
		memcpy(&free_pos, free_block.pageData, sizeof(int));
		return rslt;
	}
}

void IndexFileHead::freePage(int pos) {
	if (pos == last_pos) last_pos--;
	else {
		Page fb;
		int flag = NULL_POS;
		fb.tableName = p.tableName;
		fb.pageType = IndexPage;
		fb.ofs = pos;
		memcpy(fb.pageData, &free_pos, sizeof(int));
		memcpy(fb.pageData+sizeof(int), &flag, sizeof(int));
		bm.writePage(fb);
		free_pos = pos;
	}
}

void IndexFileHead::formatting() {

	free_pos = NULL_POS;
	root_pos = NULL_POS;
	last_pos = 0;

	memcpy(p.pageData + sizeof(char) * LABLE_POS, "Index", sizeof("Index"));
	memcpy(p.pageData + sizeof(char) * ROOT_POS, &root_pos, sizeof(root_pos));
	memcpy(p.pageData + sizeof(char) * FREE_POS, &free_pos, sizeof(free_pos));
	memcpy(p.pageData + sizeof(char) * LAST_POS, &last_pos, sizeof(last_pos));

	bm.writePage(p);
}

void IndexFileHead::reload() {
	char lable[6];

	bm.readPage(p);
	memcpy(lable, p.pageData + sizeof(char) * LABLE_POS, sizeof(lable));
	lable[5] = '\0';
	if (strcmp(lable, "Index")) {
		throw IndexException(INDEX_FILE_ERROR, "This file is not a index file!");
	}

	memcpy(&root_pos, p.pageData + sizeof(char) * ROOT_POS, sizeof(root_pos));
	memcpy(&free_pos, p.pageData + sizeof(char) * FREE_POS, sizeof(free_pos));
	memcpy(&last_pos, p.pageData + sizeof(char) * LAST_POS, sizeof(last_pos));

}

void IndexFileHead::resave() {
	memcpy(p.pageData + sizeof(char) * LABLE_POS, "Index", sizeof("Index"));
	memcpy( p.pageData + sizeof(char) * ROOT_POS, &root_pos, sizeof(root_pos));
	memcpy( p.pageData + sizeof(char) * FREE_POS, &free_pos, sizeof(free_pos));
	memcpy( p.pageData + sizeof(char) * LAST_POS, &last_pos, sizeof(last_pos));

	bm.writePage(p);
}

