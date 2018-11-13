#include "EventMessage.h"

int EventMessage::getSize() { return size; }

EventMessage::EventMessage() {
	current = first = new EventMessagePart{ 0, new char[256], 256, 4 };
	size = pos = 4;
}

EventMessage::~EventMessage() {
	auto p = first;
	while (p) {
		auto n = p->next;
		delete p->buffer;
		delete p;
		p = n;
	}
}

void EventMessage::write(char *buffer) {
	auto p = first;
	while (p) {

		memcpy(buffer, p->buffer, p->used);
		buffer += p->used;

		p = p->next;
	}
}

void EventMessage::write(int fd) {
	auto p = first;
	while (p) {

		_write(fd, p->buffer, p->used);

		p = p->next;
	}
}

void EventMessage::push_memory(void *m, unsigned int msize) {

	if (pos + msize > current->size) {
		// close current
		unsigned int psize = 256;
		if (psize < msize) {
			psize = msize;
		}
		auto p = new EventMessagePart{ 0, new char[psize], psize, 0 };
		current->next = p;
		current = p;
		pos = 0;
	}

	memcpy(current->buffer + pos, m, msize);
	pos += msize;
	current->used += msize;
	size += msize;
}

void EventMessage::push_uint8(unsigned char i) {
	push_memory(&i, 1);
}

void EventMessage::push_uint16(unsigned short i) {
	push_memory(&i, 2);
}

void EventMessage::push_uint24(unsigned long i) {
	push_memory(&i, 3);
}

void EventMessage::push_uint32(unsigned long i) {
	push_memory(&i, 4);
}

void EventMessage::push_uint64(unsigned long long i) {
	push_memory(&i, 8);
}

void EventMessage::push_string(std::string &s) {
	auto sz = s.length() + 1;

	if (sz < 256) {
		push_uint8(dt_string8);
		push_uint8((unsigned char)sz);
	}
	else if (sz < 256 * 256) {
		push_uint8(dt_string16);
		push_uint16((unsigned short)sz);
	}
	else if (sz < 256 * 256 * 256) {
		push_uint8(dt_string24);
		push_uint24((unsigned int)sz);
	}
	else {
		return;
	}

	push_memory((void *)s.c_str(), (unsigned int)s.length());
	push_uint8(0);
}

void EventMessage::push_float(float f) {
	push_uint8(dt_float);
	push_memory(&f, 4);
}

void EventMessage::push_array(int count) {

	if (count < 256) {
		push_uint8(dt_array8);
		push_uint8(count);
	}
	else if (count < 256 * 256) {
		push_uint8(dt_array16);
		push_uint16(count);
	}
	else if (count < 256 * 256 * 256) {
		push_uint8(dt_array24);
		push_uint24(count);
	}
	else {
		return;
	}

}

void EventMessage::push_int(long long i) {
	if (i >= 0) {
		push_uint(i);
		return;
	}
	i *= -1;

	if (i < 256) {
		push_uint8(dt_nint8);
		push_uint8((unsigned char)i);
	}
	else if (i < 256 * 256) {
		push_uint8(dt_nint16);
		push_uint16((unsigned short)i);
	}
	else if (i < 256 * 256 * 256) {
		push_uint8(dt_nint24);
		push_uint24((unsigned int)i);
	}
	else if (i < 256ull * 256 * 256 * 256) {
		push_uint8(dt_nint32);
		push_uint32((unsigned int)i);
	}
	else {
		push_uint8(dt_nint64);
		push_uint64(i);
	}
}

void EventMessage::push_uint(unsigned long long i) {

	if (i < 256) {
		push_uint8(dt_uint8);
		push_uint8((unsigned char)i);
	}
	else if (i < 256 * 256) {
		push_uint8(dt_uint16);
		push_uint16((unsigned short)i);
	}
	else if (i < 256 * 256 * 256) {
		push_uint8(dt_uint24);
		push_uint24((unsigned int)i);
	}
	else if (i < 256ull * 256 * 256 * 256) {
		push_uint8(dt_uint32);
		push_uint32((unsigned int)i);
	}
	else {
		push_uint8(dt_uint64);
		push_uint64(i);
	}
}

void EventMessage::push_stringid(unsigned long long i) {

	if (i < 256) {
		push_uint8(dt_string_lib8);
		push_uint8((unsigned char)i);
	}
	else if (i < 256 * 256) {
		push_uint8(dt_string_lib16);
		push_uint16((unsigned short)i);
	}
	else if (i < 256 * 256 * 256) {
		push_uint8(dt_string_lib24);
		push_uint24((unsigned int)i);
	}
	else if (i < 256ull * 256 * 256 * 254) {
		push_uint8(dt_string_lib32);
		push_uint32((unsigned int)i);
	}
	else {
		push_uint8(dt_string_lib64);
		push_uint64(i);
	}
}

void EventMessage::set_size() {
	memcpy(first->buffer, &size, 4);
}
