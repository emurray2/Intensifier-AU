#ifndef __IntensifierUnitVersion_h__
#define __IntensifierUnitVersion_h__


#ifdef DEBUG
	#define kIntensifierUnitVersion 0xFFFFFFFF
#else
	#define kIntensifierUnitVersion 0x00010000
#endif

// customized for each audio unit
#define IntensifierUnit_COMP_SUBTYPE		'intf'
#define IntensifierUnit_COMP_MANF			'Auem'

#endif

