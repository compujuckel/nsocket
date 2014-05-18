#include <os.h>
#include <ngc.h>
#include <nsocket.h>

int main(void)
{
	clrscr();
	Gc gc = *gui_gc_global_GC_ptr;
	
	// Connect to CoreTemp remote server
	if(ns_init() < 0)
	{
		show_msgbox("CoreTemp","Could not init nsocket.");
		return 1;
	}
	if(ns_connect("127.0.0.1", 5200) < 0)
	{
		show_msgbox("CoreTemp","Could not connect to CoreTemp server.");
		ns_stop();
		return 1;
	}
	
	unsigned int bufsize = ns_get_pktsize();
	
	int core_count = 0;
	while(!isKeyPressed(KEY_NSPIRE_ESC))
	{
		
		char* endptr;
		char* cpu = NULL;
		int* load = NULL;
		int* temp = NULL;
		int mem_total = 0;
		int mem_free = 0;
		int mem_percentage = 0;
		int cpu_speed = 0;
		int bclk_speed = 0;
		
		if(core_count > 0)
		{
			load = calloc(core_count, sizeof(int));
			temp = calloc(core_count, sizeof(int));
		}
		
		char utf16buffer[100] = {0};
		char textbuffer[100] = {0};
		char buffer[bufsize];
		
		int ret = ns_recv(buffer, bufsize);
		if(ret < 0)
			break;
		if(ret == 0)
			continue;
		
		char* ptr;
		char dl[] = "{}[],:\"";
		ptr = strtok(buffer,dl);
		while(ptr != NULL)
		{
			if(strcmp(ptr,"uiCoreCnt") == 0 && core_count == 0)
			{
				core_count = strtol(strtok(NULL,dl),&endptr,0);
			}
			
			if(strcmp(ptr,"CPUName") == 0)
				cpu = strtok(NULL,dl);
			
			if(strcmp(ptr,"uiLoad") == 0 && load != NULL)
			{
				int i;
				for(i = 0; i < core_count; i++)
				{
					load[i] = strtol(strtok(NULL,dl),&endptr,0);
				}
			}
			
			if(strcmp(ptr,"fTemp") == 0 && temp != NULL)
			{
				int i;
				for(i = 0; i < core_count; i++)
				{
					temp[i] = strtol(strtok(NULL,dl),&endptr,0);
				}
			}
			
			if(strcmp(ptr,"fCPUSpeed") == 0)
				cpu_speed = strtol(strtok(NULL,dl),&endptr,0);
			
			if(strcmp(ptr,"fFSBSpeed") == 0)
				bclk_speed = strtol(strtok(NULL,dl),&endptr,0);
			
			if(strcmp(ptr,"TotalPhys") == 0)
				mem_total = strtol(strtok(NULL,dl),&endptr,0);
			
			if(strcmp(ptr,"FreePhys") == 0)
				mem_free = strtol(strtok(NULL,dl),&endptr,0);
			
			if(strcmp(ptr,"MemoryLoad") == 0)
				mem_percentage = strtol(strtok(NULL,dl),&endptr,0);
			
			ptr = strtok(NULL,dl);
		}
		
		// Set up Graphics Context
		gui_gc_setRegion(gc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		gui_gc_begin(gc);
		
		gui_gc_setFont(gc, Regular11);
		
		int line_height = gui_gc_getFontHeight(gc, Regular11);
		
		gui_gc_setColor(gc, 0xFFFFFF);
		gui_gc_fillRect(gc, 0, 0, 320, 240);
		gui_gc_setColor(gc, 0x000000);
		
		// CPU type
		ascii2utf16(utf16buffer,cpu,100);
		gui_gc_drawString(gc, utf16buffer, 0, 0, GC_SM_TOP);
		
		int i;
		for(i = 0; i < core_count && load != NULL && temp != NULL; i++)
		{
			// Draw CPU load bar
			gui_gc_drawRect(gc, 0, (line_height + 2) + (25 * i), 229, 25);
			gui_gc_fillRect(gc, 1, (line_height + 3) + (25 * i), load[i] * 2.29 - 1, 24);
			
			gui_gc_drawRect(gc, 229, (line_height + 2) + (25 * i), 45, 25);
			gui_gc_drawRect(gc, 274, (line_height + 2) + (25 * i), 45, 25);

			// Print load text
			sprintf(textbuffer, "%d%%", load[i]);
			ascii2utf16(utf16buffer, textbuffer, 100);
			int width = gui_gc_getStringWidth(gc, Regular11, utf16buffer, 0, strlen(textbuffer));
			gui_gc_drawString(gc, utf16buffer, 230 + (43 - width) / 2,  line_height + 2 + (23 - line_height) / 2 + (25 * i), GC_SM_TOP);
			
			// Print temperature
			sprintf(textbuffer, "%dC", temp[i]);
			ascii2utf16(utf16buffer, textbuffer, 100);
			width = gui_gc_getStringWidth(gc, Regular11, utf16buffer, 0, strlen(textbuffer));
			gui_gc_drawString(gc, utf16buffer, 275 + (43 - width) / 2, line_height + 2 + (23 - line_height) / 2 + (25 * i), GC_SM_TOP);
		}
		
		// Print CPU speed
		gui_gc_drawRect(gc, 0, (line_height + 2) + (25 * core_count), 160, 25);
		sprintf(textbuffer, "CPU: %d MHz", cpu_speed);
		ascii2utf16(utf16buffer, textbuffer, 100);
		gui_gc_drawString(gc, utf16buffer, 3, line_height + 2 + (23 - line_height) / 2 + (25 * core_count), GC_SM_TOP);
		
		// Print BCLK speed
		gui_gc_drawRect(gc, 160, (line_height + 2) + (25 * core_count), 159, 25);
		sprintf(textbuffer, "BCLK: %d MHz", bclk_speed);
		ascii2utf16(utf16buffer, textbuffer, 100);
		gui_gc_drawString(gc, utf16buffer, 163, line_height + 2 + (23 - line_height) / 2 + (25 * core_count), GC_SM_TOP);
		
		// Draw memory load bar
		gui_gc_drawRect(gc, 0, (line_height + 2) + (25 * (core_count + 1)), 274, 25);
		gui_gc_fillRect(gc, 1, (line_height + 3) + (25 * (core_count + 1)), mem_percentage * 2.74 - 1, 24);
		
		gui_gc_drawRect(gc, 274, (line_height + 2) + (25 * (core_count + 1)), 45, 25);
		
		// Print temperature
		sprintf(textbuffer, "%d%%", mem_percentage);
		ascii2utf16(utf16buffer, textbuffer, 100);
		int width = gui_gc_getStringWidth(gc, Regular11, utf16buffer, 0, strlen(textbuffer));
		gui_gc_drawString(gc, utf16buffer, 275 + (43 - width) / 2, line_height + 2 + (23 - line_height) / 2 + (25 * (core_count + 1)), GC_SM_TOP);
		
		gui_gc_drawRect(gc, 0, (line_height + 2) + (25 * (core_count + 2)), 319, 25);
		sprintf(textbuffer, "RAM: %dMB of %dMB used", mem_total - mem_free, mem_total);
		ascii2utf16(utf16buffer, textbuffer, 100);
		gui_gc_drawString(gc, utf16buffer, 3, line_height + 2 + (23 - line_height) / 2 + (25 * (core_count + 2)), GC_SM_TOP);
		
		gui_gc_finish(gc);
		gui_gc_blit_to_screen(gc);
		
		free(load);
		free(temp);
	}
	
	ns_stop();
	return 0;
}