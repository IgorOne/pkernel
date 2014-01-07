#include <vga.h>

static u16 *_vga_mem;
static u8 _vga_t_color;
static u16 _vga_t_row;
static u16 _vga_t_colm;

u8 vga_make_color(enum vga_color fg, enum vga_color bg)
{
	return fg|bg<<4;
}

static u16 _vga_make_entry(char c, u8 color)
{
	u16 c16 = c;
	u16 color16 = color;
	return c16|color16<<8;
}

void vga_set_color(u8 color)
{
	_vga_t_color = color;
}

void vga_putchar(char c)
{
	if(c == '\n') {
		_vga_t_row++;
		_vga_t_colm = -1;
	} else if(c >= ' ') {
		const size_t index = _vga_t_row*VGA_WIDTH+_vga_t_colm;
		_vga_mem[index] = _vga_make_entry(c, _vga_t_color);
	}

	if(++_vga_t_colm == VGA_WIDTH) {
		_vga_t_colm = 0;
		if(++_vga_t_row == VGA_HEIGHT) {
			_vga_t_row = 0;
		}
	}
}

void vga_move_cursor()
{
	u16 pos = _vga_t_row*VGA_WIDTH+_vga_t_colm;
	outb(0x3D4, 14);
	outb(0x3D5, pos>>16);
	outb(0x3D4, 15);
	outb(0x3D5, pos);
}

void vga_clear()
{
	_vga_t_row = 0;
	_vga_t_colm = 0;
	_vga_t_color = vga_make_color(COLOR_LIGHT_GREY, COLOR_BLACK);

	for(size_t y = 0; y < VGA_HEIGHT; y++) {
		for(size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y*VGA_WIDTH+x;
			_vga_mem[index] = _vga_make_entry(' ', _vga_t_color);
		}
	}

	vga_move_cursor();
}

void vga_write(const char *str)
{
	size_t i = 0;
	while(str[i])
		vga_putchar(str[i++]);

	vga_move_cursor();
}

void vga_write_hex(uint32_t n)
{
	char buf[9];
	itoa(buf, n, 16);
	vga_write(buf);
}

void vga_write_dec(uint32_t n)
{
	char buf[11];
	itoa(buf, n, 10);
	vga_write(buf);
}

void vga_init(u16 *vmem)
{
	_vga_mem = vmem;
	vga_clear();
}