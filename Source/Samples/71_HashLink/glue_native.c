#define HL_NAME(n) urho3d_##n

#include <hl.h>
#include <stdarg.h>
#include <SDL/SDL.h>


void hashlink_urho3d_log(char * str);

int hl_ustrlen_utf8( const uchar *str ) {
	int size = 0;
	while(1) {
		uchar c = *str++;
		if( c == 0 ) break;
		if( c < 0x80 )
			size++;
		else if( c < 0x800 )
			size += 2;
		else if( c >= 0xD800 && c <= 0xDFFF ) {
			str++;
			size += 4;
		} else
			size += 3;
	}
	return size;
}

// USE UTF-8 encoding
int hl_utostr( char *out, int out_size, const uchar *str ) {
	char *start = out;
	char *end = out + out_size - 1; // final 0
	if( out_size <= 0 ) return 0;
	while( out < end ) {
		unsigned int c = *str++;
		if( c == 0 ) break;
		if( c < 0x80 )
			*out++ = (char)c;
		else if( c < 0x800 ) {
			if( out + 2 > end ) break;
			*out++ = (char)(0xC0|(c>>6));
			*out++ = 0x80|(c&63);
		} else if( c >= 0xD800 && c <= 0xDFFF ) { // surrogate pair
			if( out + 4 > end ) break;
			unsigned int full = (((c - 0xD800) << 10) | ((*str++) - 0xDC00)) + 0x10000;
			*out++ = (char)(0xF0|(full>>18));
			*out++ = 0x80|((full>>12)&63);
			*out++ = 0x80|((full>>6)&63);
			*out++ = 0x80|(full&63);
		} else {
			if( out + 3 > end ) break;
			*out++ = (char)(0xE0|(c>>12));
			*out++ = 0x80|((c>>6)&63);
			*out++ = 0x80|(c&63);
		}
	}
	*out = 0;
	return (int)(out - start);
}

static char *hl_utos( const uchar *s ) {
	int len = hl_ustrlen_utf8(s);
	char *out = (char*)malloc(len + 1);
	if( hl_utostr(out,len+1,s) < 0 )
		*out = 0;
	return out;
}

int my_thread( void *data )
{

    printf("started my_thread \n");

    vclosure *callback_fn = (vclosure *)data;

    vdynamic * dyn = hl_alloc_dynamic( &hlt_i32 );
    vdynamic *args[1];

    for(int i = 50 ; i < 60;i++)
    {
        dyn->v.i = i; 
        args[0] = dyn;
        hl_dyn_call(callback_fn, args, 1);
        SDL_Delay(1000);
    }

    dyn->v.i = -1; 
    args[0] = dyn;
    hl_dyn_call(callback_fn, args, 1);

    hl_remove_root(&callback_fn);

    return 0;
}

int fibR(int n)
{
    if (n < 2)
        return n;
    return (fibR(n - 2) + fibR(n - 1));
}

int add_numbers(int a, int b)
{
    return a + b;
}

HL_PRIM int HL_NAME(w_add_numbers)(int a, int b)
{
    return add_numbers(a, b);
}

HL_PRIM int HL_NAME(fib)(int a)
{
    return fibR(a);
}

HL_PRIM void HL_NAME(tick)(int a)
{
    printf("tick: %d \n", a);
}

HL_PRIM void HL_NAME(pass_callback)(vclosure *callback_fn)
{
    hl_add_root(&callback_fn);

   hl_thread * thread = hl_thread_start(my_thread, callback_fn, true);
   
   // hl_remove_root(&callback_fn);
}

HL_PRIM void HL_NAME(trace)( vstring *msg )
{
    char *cstr = hl_utos((uchar*) msg->bytes);
    //hashlink_urho3d_log(cstr);
    
    SDL_Log("%s",cstr);

}

DEFINE_PRIM(_I32, w_add_numbers, _I32 _I32);
DEFINE_PRIM(_I32, fib, _I32);
DEFINE_PRIM(_VOID, tick, _I32);
DEFINE_PRIM(_VOID, pass_callback, _FUN(_VOID, _I32));
DEFINE_PRIM(_VOID, trace, _STRING);
//
