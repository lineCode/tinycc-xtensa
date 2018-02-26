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
	/* XXX XXX TODO THIS CODE IS ALMOST CERTAINLY WRONG
	   Can return BUILD_GOT_ONLY, NO_GOTPLT_ENTRY or ALWAYS_GOTPLT_ENTRY
	   ... OR -1 if a problem. */

	switch( reloc_type )
	{
	case R_JS_CODE_ABS32:
		return ALWAYS_GOTPLT_ENTRY;
	case R_JS_DATA_ABS32:
		return AUTO_GOTPLT_ENTRY;
	}
	return -1;
	#if 0
		/* Whether to generate a GOT/PLT entry and when. NO_GOTPLT_ENTRY is first so
		   that unknown relocation don't create a GOT or PLT entry */
		enum gotplt_entry {
		    NO_GOTPLT_ENTRY,	/* never generate (eg. GLOB_DAT & JMP_SLOT relocs) */
		    BUILD_GOT_ONLY,	/* only build GOT (eg. TPOFF relocs) */
		    AUTO_GOTPLT_ENTRY,	/* generate if sym is UNDEF */
		    ALWAYS_GOTPLT_ENTRY	/* always generate (eg. PLTOFF relocs) */
		};
	#endif
}

/* This function seems to be used any time you have a function definition, if you force
   ALWAYS_GOTPLT_ENTRY for code gotplt_entry_type. */
ST_FUNC unsigned create_plt_entry(TCCState *s1, unsigned got_offset, struct sym_attr *attr)
{
	uint8_t *p;
	Section *plt = s1->plt;
	unsigned plt_offset, relofs;

	printf( "=====================CREATING PLT ENTRY\n");

	/* Should probably check to see if s1->output_type == TCC_OUTPUT_DLL.
	   if so, should probably handle this by adding an offset to start of
	   GOT table. */

	if (plt->data_offset == 0) {
		p = section_ptr_add(plt, 16);
		p[0] = 1;
		p[1] = 2;
		p[2] = 3;
		p[3] = 4;
	}
	plt_offset = plt->data_offset;

	/* The PLT slot refers to the relocation entry it needs via offset.
	The reloc entry is created below, so its offset is the current
	data_offset */
	relofs = s1->got->reloc ? s1->got->reloc->data_offset : 0;

	/* Jump to GOT entry where ld.so initially put the address of ip + 4 */
	p = section_ptr_add(plt, 16);
	p[0] = relofs>>0;
	p[1] = relofs>>8;
	p[2] = relofs>>16;
	p[3] = relofs>>24;
	p[4] = 0xaa;
	p[5] = 0x55;
	return plt_offset;
}

/* relocate the PLT: compute addresses and offsets in the PLT now that final
   address for PLT and GOT are known (see fill_program_header) */
ST_FUNC void relocate_plt(TCCState *s1)
{
    uint8_t *p, *p_end;

    if (!s1->plt)
      return;

    printf( "===========================Warning: relocate_plt not implemented.\n" );

    p = s1->plt->data;
    p_end = p + s1->plt->data_offset;

    if (p < p_end) {
        /* XXX: TODO */
        while (p < p_end) {
            /* XXX: TODO */
        }
   }

}



static ElfW_Rel *qrel; /* ptr to next reloc entry reused */
static Section *srl;

void relocate_init(Section *sr)
{
	qrel = (ElfW_Rel *) sr->data;
	srl = sr;
	printf( "Initializing relocation\n" );
}

void relocate(TCCState *s1, ElfW_Rel *rel, int type, unsigned char *ptr, addr_t addr, addr_t val)
{

	int sym_index = ELFW(R_SYM)(rel->r_info);     //Not sure what this is.
	ElfW(Sym) *sym;
	sym = &((ElfW(Sym) *)symtab_section->data)[sym_index];

//	printf( ":: %p %p\n", sym, sym_index );

	int esym_index;
	uint32_t place_to_patch = rel->r_offset;  //Offset from beginning of text section.
	//uint8_t  * ptr_to_patch = ptr;            //The actual start of the line that needs patching.

	// Set the new patch value to "val"

	
//offset != rel->r_offset - s1->got->sh_addr

	printf( "Do relocation %p %p %d %p [%d %d] ---> R_OFF: %p  SYM INDEX: %d  SEC: %p\n", s1, rel, type, ptr, addr, val, rel->r_offset, sym_index, srl->sh_addr );
	printf( "GOTAD: %p\n", s1->got->sh_addr );

	//Not sure.  Why do we have to do this?
	struct sym_attr *attr = get_sym_attr(s1, sym_index, 0);
	attr->got_offset = s1->got->sh_addr-rel->r_offset;
//	printf( "%d = %d - %d\n", attr->got_offset, rel->r_offset, s1->got->sh_addr );

//	printf( "ADDY %p %p\n", s1->plt->data, s1->plt->data_offset );
//	printf( "%c%c%c%c%c%c%c%c\n", ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5], ptr[6], ptr[7] );
	
#if 1
	switch( type )
	{
	case R_JS_CODE_ABS32:
		printf( "Relocate code\n" );
		printf( "%c%c%c%c%c%c%c%c\n", ptr[0],ptr[1],ptr[2],ptr[3],ptr[4],ptr[5],ptr[6],ptr[7] );
		break;
	case R_JS_DATA_ABS32:
		{
			printf( " name=%s\n", (char *) symtab_section->link->data + sym->st_name);
			char buff[11];
			sprintf( buff, "%08x", val );
			memcpy( ptr, buff, 8 );
		}
		break;
	}
#endif

}

#endif /* !TARGET_DEFS_ONLY */
