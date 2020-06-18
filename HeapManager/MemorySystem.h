#pragma once
namespace GStar {
	// InitializeMemorySystem - initialize your memory system including your HeapManager and some FixedSizeAllocators
	bool InitializeMemorySystem();

	// Collect - coalesce free blocks in attempt to create larger blocks
	void Collect();

	// DestroyMemorySystem - destroy your memory systems
	void DestroyMemorySystem();
}
