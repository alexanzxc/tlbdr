
#include <pgtable.h>
#include "mm_locking.h"

int resolve_va(size_t addr, struct ptwalk *entry, int lock)
{
	struct mm_struct *mm;
	pgd_t *pgd;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
	p4d_t *p4d;
#else
	size_t *p4d;
#endif
	pud_t *pud;
	pmd_t *pmd;
	//pte_t *pte;

	// page table walk entry
	if (!entry)
		return -EINVAL;

	entry->pgd = NULL;
	entry->p4d = NULL;
	entry->pud = NULL;
	entry->pmd = NULL;
	//entry->pte = NULL;
	entry->valid = 0;

	if (!current->mm)
		return -EINVAL;

	if (lock)
		down_read(TLBDR_MMLOCK); // mmlock , down_read kernel semaphore

	// page table levels offsets, kernel funcs
	pgd = pgd_offset(current->mm, addr);

	if (pgd_none(*pgd) || pgd_bad(*pgd))
		goto err;

	entry->pgd = pgd;
	entry->valid |= MMUCTL_PGD; // bitwise OR, 0 OR 1

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
	p4d = p4d_offset(entry->pgd, addr);

	if (p4d_none(*p4d) || p4d_bad(*p4d))
		goto err;

	entry->p4d = p4d;
	entry->valid |= MMUCTL_P4D; // bitwise OR, 0 OR 2

	pud = pud_offset(entry->p4d, addr);
#else
	pud = pud_offset(pgd, addr);
#endif
	if (pud_none(*pud) || pud_bad(*pud))
		goto err;

	entry->pud = pud;
	entry->valid |= MMUCTL_PUD; // bitwise OR, 0 OR 4

	pmd = pmd_offset(pud, addr);

	if (pmd_none(*pmd) || pmd_bad(*pmd))
		goto err;

	entry->pmd = pmd;
	//entry->valid = MMUCTL_PMD; // why no OR ? 8
	entry->valid |= MMUCTL_PMD;
	

	//pte = pte_offset_map(pmd, addr); // study func
	// vkarakos: unclear how to handle this -- need to check
    //pmd_unmap(pmd);
    pte_unmap(pmd);

	// entry->pte = pte;
	// entry->valid |= MMUCTL_PTE; // bitwise OR, 0 OR 16

	// pte_unmap(pte);

	if (lock)
		up_read(TLBDR_MMLOCK);

	return 0;

err:
	if (lock)
		up_read(TLBDR_MMLOCK);

	return -1;
}

/* NXBIT It allows to mark each memory page as being
 "allowed" or "disallowed" for code execution.
 This function sets it to 0 probably?*/
void clear_nx(pgd_t *p)
{
	// vkarakos: should we change this?
	p->pgd &= ~NXBIT;
}
