#ifdef TARGET_DEFS_ONLY

#define EM_TCC_TARGET EM_C60

/* relocation type for 32 bit data relocation */
#define R_DATA_32   R_C60_32
#define R_DATA_PTR  R_C60_32
#define R_JMP_SLOT  R_C60_JMP_SLOT
#define R_GLOB_DAT  R_C60_GLOB_DAT
#define R_COPY      R_C60_COPY
#define R_RELATIVE  R_C60_RELATIVE

#define R_NUM       R_C60_NUM

#define ELF_START_ADDR 0x00000400
#define ELF_PAGE_SIZE  0x1000

#define PCRELATIVE_DLLPLT 0
#define RELOCATE_DLLPLT 0

#else /* !TARGET_DEFS_ONLY */

#include "tcc.h"

/* Returns 1 for a code relocation, 0 for a data relocation. For unknown
   relocations, returns -1. */
int code_reloc (int reloc_type)
{
	printf( "CODE RELOC %d\n", reloc_type );
	switch (reloc_type)
	{
	case R_JS_CODE_ABS32:
		return 1;
	case R_JS_DATA_ABS32:
		return 0;
	}

	tcc_error ("Unknown relocation type: %d", reloc_type);
	return -1;
}

/* Returns an enumerator to describe whether and when the relocation needs a
   GOT and/or PLT entry to be created. See tcc.h for a description of the
   different values. */
int gotplt_entry_type (int reloc_type)
{

	/* 
		Normally, when you have applications, they are expected to do some
		run-time lazy linking. However, in our example, and my target for the
		xtensa compiler will never do lazy linking.  I expect the executable
		loader to patch all relocations before execution begins. For this
		reason we would never use the PLT entries. 

		TODO: WRT GOT entries: when would/wouldn't they be used?

			NO_GOTPLT_ENTRY
				and
			BUILD_GOT_ONLY

		are the only two options we should be using in this case.
	*/

	switch( reloc_type )
	{
	case R_JS_CODE_ABS32:
		return NO_GOTPLT_ENTRY;
	case R_JS_DATA_ABS32:
		return NO_GOTPLT_ENTRY;
	}
	return -1;
}

/* This function seems to be used any time you have a function definition, if you force
   ALWAYS_GOTPLT_ENTRY for code gotplt_entry_type. */
ST_FUNC unsigned create_plt_entry(TCCState *s1, unsigned got_offset, struct sym_attr *attr)
{
	tcc_error( "PLT tables are not implemented for the javascript target." );
	return 0;
}

/* relocate the PLT: compute addresses and offsets in the PLT now that final
   address for PLT and GOT are known (see fill_program_header) */
ST_FUNC void relocate_plt(TCCState *s1)
{	tcc_error( "PLT tables are not implemented for the javascript target." );
}



static ElfW_Rel *qrel; /* ptr to next reloc entry reused */
static Section *srl;

void relocate_init(Section *sr)
{
	qrel = (ElfW_Rel *) sr->data;
	srl = sr;

	printf( "Initializing relocation %p\n", sr->sh_addr );
}

void relocate(TCCState *s1, ElfW_Rel *rel, int type, unsigned char *ptr, addr_t addr, addr_t val)
{
	int esym_index;
	int sym_index = ELFW(R_SYM)(rel->r_info); //Not sure what this is.
	uint32_t place_to_patch = rel->r_offset;  //Offset from beginning of text section.
	uint8_t  * ptr_to_patch = ptr;            //The actual start of the line that needs patching.
#if 1
	printf( "Do relocation %p %p %d %p [%d %d] ---> %p  SYM INDEX: %d  SEC: %p %p\n",
		s1, rel, type, ptr,   addr, val, rel->r_offset, sym_index, srl->sh_addr, symtab_section->sh_addr );
#endif

#if 1
	switch( type )
	{
	case R_JS_CODE_ABS32:
		//Code relocation doesn't actually exist in the javascript target.
		//The symbol was just emitted for the sake of making objdump show it.
		break;
	case R_JS_DATA_ABS32:
		{
			char cts[10];
			if( val == 0 ) val = 0xffffffff;
			else
				val -= s1->sections[1]->sh_addr + 4; //XXX awful hack to ignore segment offset. XXX This will probably break!!!

			sprintf( cts, "%08x", val );
			memcpy( ptr_to_patch, cts, 8 );
		}
		break;
	}
#endif
}

#endif /* !TARGET_DEFS_ONLY */

//XXX PICK UP HERE.
//TODO from here: See how loading the elf will patch the 0xffffffff's.  We need to make sure we have a mechansism for this.
