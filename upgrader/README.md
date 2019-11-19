# STM32 Upgrader

## Memory layout

```
----------------- 0x08000000
|     loader    | --> 28K
----------------- 0x08007000
|      EFIT     | --> 4K
----------------- 0x08008000
|  application  | --> 28K
----------------- 0x0800f000
|    upgrader   | --> 4K
----------------- 0x08010000
```

## EFIT -- Executable File Information Table

Executable file information table is consist of executable file information entries (EFIE).  
Each entry is 128 bytes long, it is equal to a page of stm32 program flash.  
Entry can be described by c code like below.  

```
struct efie {
	uint32_t	offset;
	uint32_t	length;
	uint8_t		checksum[16];
	uint8_t		reserved[104];
};
```

The first entry is application's information block. The second one is upgrader's information block.  
So application offset and upgrader offset can be changed, but EFIT must locates in fixed offset, 28K from program flash.  
But for compatible with former memory layout, we normally put application at 32K offset and upgrader at 60K offset.  

## checksum
We need a method to check upgrade integrate. loader should check application's checksum value to see if we have a broken upgrade.  
The checksum algrithm is a private one for speed consideration. It XOR each 16 bytes data with the previous one. The first block XOR with a initial block. None aligned data is processed in byte and no padding should performed.  
In c code it is  

```
void checksum(void *out, void *in, unsigned long len)
{
	uint32_t *src = in;
	char *init = "*BITMAIN-SOPHON*";
	uint32_t result[4];
	memcpy(result, init, sizeof(result));
	
	unsigned long block = len >> 4;
	unsigned long left = len & 0xf;
	unsigned long i, j;

	for (i = 0; i < block; ++i, src += 4) {
		for (j = 0; j < 4; ++j) {
			result[j] ^= src[j];
		}
	}

	for (i = 0; i < left; ++i) {
		((uint8_t *)result)[i] ^= ((uint8_t *)src)[i];
	}
	memcpy(out, result, sizeof(result));
}
```

## Boot flow
* Loader bootup
* Loader get application's information from EFIT first entry
* Loader check application's integrity using checksum
* If application is valid, then jump to application
* Else wait user to perform another fixed upgrade

## Loader and application
Loader and application is compiled using the save code. So it can take the role of application.  
The only difference is that they run at different address (application is not position independet).  
We add a register 0x1B in MCU i2c register map to identify am I in loader or application.  
If we are in loader which means we have a broken upgrade, try to upgrade application again to fix it.
