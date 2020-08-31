#include <inc/memlayout.h>
#include <kern/kheap.h>
#include <kern/memory_manager.h>

//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
struct freeframinfo
{
	int numofpages;
	uint32 va;

};
uint32 firstfreeadd;
struct freeframinfo allocated_frames[(KERNEL_HEAP_MAX-KERNEL_HEAP_START)/PAGE_SIZE];
struct  freeframinfo free_frames[(KERNEL_HEAP_MAX-KERNEL_HEAP_START)/PAGE_SIZE];
int index_of_allocated_frames=0;

void* kmalloc(unsigned int size)
{

	//TODO: [PROJECT 2019 - MS1 - [1] Kernel Heap] kmalloc()
	// Write your code here, remove the panic and write your code
	//kpanic_into_prompt("kmalloc() is not implemented yet...!!");
    //cprintf("enter kmalloc\n");
	int rounded_size=ROUNDUP(size,PAGE_SIZE);
	int num_of_reqpages=rounded_size/PAGE_SIZE;
	uint32 start_address;
	int count=0,index=-1,min_index=-1,min_pages=((KERNEL_HEAP_MAX-KERNEL_HEAP_START)/PAGE_SIZE)+1;
	if(isKHeapPlacementStrategyBESTFIT())
	{
	for(uint32 i=KERNEL_HEAP_START;i<KERNEL_HEAP_MAX;i+=PAGE_SIZE)
	{
		uint32 *page_tab=NULL;
	    struct Frame_Info * fram=get_frame_info(ptr_page_directory,(void*)i,&page_tab);
	    if(fram==NULL)
	    {
	    	if(count==0)
	    	{
	    		//cprintf("count=0");
	    		index++;
	    		firstfreeadd=i;
	    		free_frames[index].va=firstfreeadd;
	    	}
	    	count++;
	    	free_frames[index].numofpages=count;
	    }
	    else
	    {
	    	count=0;

	    }
	}
	//cprintf("malina al array\n");

	for(int k=0;k<=index;k++)
	{
		if(num_of_reqpages<=free_frames[k].numofpages)
		{
			if(min_pages>free_frames[k].numofpages)
			{
				min_pages=free_frames[k].numofpages;
				min_index=k;
			}
		}

	}

	//cprintf("l2aina al min_index \n");
	if(min_index==-1)
	{
		return NULL;
	}

	}
	else if(isKHeapPlacementStrategyFIRSTFIT())
	{
		for(uint32 i=KERNEL_HEAP_START;i<KERNEL_HEAP_MAX;i+=PAGE_SIZE)
			{
				uint32 *page_tab=NULL;
			    struct Frame_Info * fram=get_frame_info(ptr_page_directory,(void*)i,&page_tab);
			    if(fram==NULL)
			    {
			    	if(count==0)
			    	{
			    		//cprintf("count=0");
			    		index++;
			    		firstfreeadd=i;
			    		free_frames[index].va=firstfreeadd;
			    	}
			    	count++;
			    	free_frames[index].numofpages=count;
			    }
			    else
			    {
			    	count=0;

			    }
			}
		for(int k=0;k<=index;k++)
			{
				if(free_frames[k].numofpages>=num_of_reqpages)
				{

						min_index=k;
						break;

				}

			}
		//cprintf("%d \n",min_index);
		if(min_index==-1)
		{
		   return NULL;
		}


	}
	start_address=free_frames[min_index].va;
	allocated_frames[index_of_allocated_frames].va=start_address;
	allocated_frames[index_of_allocated_frames].numofpages=num_of_reqpages;
	index_of_allocated_frames++;
	while(num_of_reqpages>0)
	{
		 uint32 *ptr_table=NULL;
		 struct Frame_Info *ptr_fram_info= NULL;
		 allocate_frame(&ptr_fram_info);
		 map_frame(ptr_page_directory,ptr_fram_info,(void *)start_address,PERM_WRITEABLE|PERM_PRESENT);
		 num_of_reqpages-=1;
		 start_address+=PAGE_SIZE;
		 firstfreeadd+=PAGE_SIZE;
	}
	//cprintf("b3d al allocat\n");
	return (void*)free_frames[min_index].va;


}

void kfree(void* virtual_address)
{
	//TODO: [PROJECT 2019 - MS1 - [1] Kernel Heap] kfree()
	// Write your code here, remove the panic and write your code
	//panic("kfree() is not implemented yet...!!");
	int index=-1;
	for(int i=0;i<index_of_allocated_frames;i++)
	{
		if((void*)allocated_frames[i].va== virtual_address)
		{
		   index=i;
		   break;
		}
	}
	//uint32 *page_tab;
	uint32 va;
	if(index==-1)
	{
		return;
	}
	for(int i=0;i<allocated_frames[index].numofpages;i++)
	{
		 va = (int32)virtual_address+(i*PAGE_SIZE);
		 unmap_frame(ptr_page_directory, (void*) va);
		 //get_page_table(ptr_page_directory,(void*)va,&page_tab);
	}

	for(int i=index;i<index_of_allocated_frames-1;i++)
	{
		allocated_frames[i].va=allocated_frames[i+1].va;
		allocated_frames[i].numofpages=allocated_frames[i+1].numofpages;
	}
	index_of_allocated_frames-=1;
	//you need to get the size of the given allocation using its address
	//refer to the project presentation and documentation for details

}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT 2019 - MS1 - [1] Kernel Heap] kheap_virtual_address()
	// Write your code here, remove the panic and write your code
	//panic("kheap_virtual_address() is not implemented yet...!!");

	//return the virtual address corresponding to given physical_address
	//refer to the project presentation and documentation for details

	//change this "return" according to your answer
	uint32*page_table;
    struct Frame_Info* frame_info = NULL;
    uint32 pa,va=0;
	for(uint32 i=KERNEL_HEAP_START;i<KERNEL_HEAP_MAX;i+=PAGE_SIZE)
		{

			frame_info = get_frame_info(ptr_page_directory,(void *)i,&page_table);
			if(frame_info != NULL)
			{
				pa = to_physical_address(frame_info);
				if(pa == physical_address)
				{
					va=i;
					break;
				}
			}
		}

	return va;
}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//TODO: [PROJECT 2019 - MS1 - [1] Kernel Heap] kheap_physical_address()
	// Write your code here, remove the panic and write your code
	//panic("kheap_physical_address() is not implemented yet...!!");

	uint32 *page_table=NULL;
    get_page_table(ptr_page_directory,(void*)virtual_address,&page_table);
	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details

	//change this "return" according to your answer
    unsigned int pa=page_table[PTX(virtual_address)];
		return (pa>>12)*PAGE_SIZE;

}


//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().

void *krealloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT 2019 - BONUS2] Kernel Heap Realloc
	// Write your code here, remove the panic and write your code
	int rounded_size=ROUNDUP(new_size,PAGE_SIZE);
	int num_of_reqpages=rounded_size/PAGE_SIZE;
  if(virtual_address==NULL)
  {
	  return (void*)kmalloc(new_size);
  }
  if(new_size==0)
  {
	  kfree(virtual_address);
	  return NULL;
  }

  for(int i=0;i<index_of_allocated_frames;i++)
  {
	  if(allocated_frames[i].va==(uint32)virtual_address)
	  {
		  if(allocated_frames[i].numofpages>=num_of_reqpages)
		  {
			  return virtual_address;
		  }
		  else
		  {
			  int isbreak=0;
			  int count=0;
			  int diff=num_of_reqpages-allocated_frames[i].numofpages;
			  uint32 va=(uint32)virtual_address+(allocated_frames[i].numofpages*PAGE_SIZE);
			  uint32 start_address=va;
			  while(count<diff)
			  {
				  uint32 *page_tab=NULL;
				  struct Frame_Info * fram=get_frame_info(ptr_page_directory,(void*)va,&page_tab);
				  if(fram!=NULL)
				  {
					  isbreak=1;
					  break;
				  }
				  count++;
				  va+=PAGE_SIZE;
			  }

			  if(isbreak==0)
			  {

				  allocated_frames[i].numofpages=num_of_reqpages;
				  while(diff>0)
					{
						 uint32 *ptr_table=NULL;
						 struct Frame_Info *ptr_fram_info= NULL;
						 allocate_frame(&ptr_fram_info);
						 map_frame(ptr_page_directory,ptr_fram_info,(void *)start_address,PERM_WRITEABLE|PERM_PRESENT);
						 diff-=1;
						 start_address+=PAGE_SIZE;
					}

				  return virtual_address;
			  }
			  else if(isbreak==1)
			  {
				  void*newva=kmalloc(new_size);

				  if(newva==NULL)
				  {

					  return NULL;
				  }
				  uint32*retva=(uint32*)newva;
				  uint32*realva=(uint32 *)virtual_address;
				  for(int j=0;j<allocated_frames[i].numofpages;j++)
				  {
					  for(int k=0;k<1024;k++)
					  {
						 *retva=*realva;
						  retva++;
						  realva++;
					  }
				  }
				  kfree(virtual_address);
				  return (void*)newva;
			  }

		  }
	  }
  }
	return NULL;


}
