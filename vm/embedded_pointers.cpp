#include "master.hpp"

namespace factor
{

/* Load a 32-bit value from a PowerPC LIS/ORI sequence */
fixnum embedded_pointer::load_address_2_2()
{
	cell *ptr = (cell *)pointer;
	cell hi = (ptr[-1] & 0xffff);
	cell lo = (ptr[ 0] & 0xffff);
	return hi << 16 | lo;
}

/* Load a value from a bitfield of a PowerPC instruction */
fixnum embedded_pointer::load_address_masked(cell mask, fixnum shift)
{
	cell *ptr = (cell *)pointer;

	return (*ptr & mask) << shift;
}

fixnum embedded_pointer::load_address()
{
	switch(rel_class)
	{
	case RC_ABSOLUTE_CELL:
		return *(cell *)pointer;
	case RC_ABSOLUTE:
		return *(u32*)pointer;
	case RC_RELATIVE:
		return *(u32*)pointer + pointer + sizeof(u32);
	case RC_ABSOLUTE_PPC_2_2:
		return load_address_2_2();
	case RC_ABSOLUTE_PPC_2:
		return load_address_masked(rel_absolute_ppc_2_mask,0);
	case RC_RELATIVE_PPC_2:
		return load_address_masked(rel_relative_ppc_2_mask,0) + pointer;
	case RC_RELATIVE_PPC_3:
		return load_address_masked(rel_relative_ppc_3_mask,0) + pointer;
	case RC_RELATIVE_ARM_3:
		return load_address_masked(rel_relative_arm_3_mask,2) + pointer + sizeof(cell) * 2;
	case RC_INDIRECT_ARM:
		return load_address_masked(rel_indirect_arm_mask,0) + pointer + sizeof(cell);
	case RC_INDIRECT_ARM_PC:
		return load_address_masked(rel_indirect_arm_mask,0) + pointer + sizeof(cell) * 2;
	default:
		critical_error("Bad rel class",rel_class);
		return 0;
	}
}

/* Store a 32-bit value into a PowerPC LIS/ORI sequence */
void embedded_pointer::store_address_2_2(fixnum value)
{
	cell *ptr = (cell *)pointer;
	ptr[-1] = ((ptr[-1] & ~0xffff) | ((value >> 16) & 0xffff));
	ptr[ 0] = ((ptr[ 0] & ~0xffff) | (value & 0xffff));
}

/* Store a value into a bitfield of a PowerPC instruction */
void embedded_pointer::store_address_masked(fixnum value, cell mask, fixnum shift)
{
	cell *ptr = (cell *)pointer;

	/* This is unaccurate but good enough */
	fixnum test = (fixnum)mask >> 1;
	if(value <= -test || value >= test)
		critical_error("Value does not fit inside relocation",0);

	*ptr = ((*ptr & ~mask) | ((value >> shift) & mask));
}

void embedded_pointer::store_address(fixnum absolute_value)
{
	fixnum relative_value = absolute_value - pointer;

	switch(rel_class)
	{
	case RC_ABSOLUTE_CELL:
		*(cell *)pointer = absolute_value;
		break;
	case RC_ABSOLUTE:
		*(u32*)pointer = absolute_value;
		break;
	case RC_RELATIVE:
		*(u32*)pointer = relative_value - sizeof(u32);
		break;
	case RC_ABSOLUTE_PPC_2_2:
		store_address_2_2(absolute_value);
		break;
	case RC_ABSOLUTE_PPC_2:
		store_address_masked(absolute_value,rel_absolute_ppc_2_mask,0);
		break;
	case RC_RELATIVE_PPC_2:
		store_address_masked(relative_value,rel_relative_ppc_2_mask,0);
		break;
	case RC_RELATIVE_PPC_3:
		store_address_masked(relative_value,rel_relative_ppc_3_mask,0);
		break;
	case RC_RELATIVE_ARM_3:
		store_address_masked(relative_value - sizeof(cell) * 2,rel_relative_arm_3_mask,2);
		break;
	case RC_INDIRECT_ARM:
		store_address_masked(relative_value - sizeof(cell),rel_indirect_arm_mask,0);
		break;
	case RC_INDIRECT_ARM_PC:
		store_address_masked(relative_value - sizeof(cell) * 2,rel_indirect_arm_mask,0);
		break;
	default:
		critical_error("Bad rel class",rel_class);
		break;
	}
}

}
