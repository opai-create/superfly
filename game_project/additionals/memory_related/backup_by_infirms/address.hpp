#pragma once

#include <Windows.h>

// class size is only 4 bytes on x86-32 and 8 bytes on x86-64.
class Address {
protected:
	uintptr_t m_addr;
public:
	// default ctor/dtor.
	__forceinline  Address() : m_addr{} {};
	__forceinline ~Address() {};

	// ctors.
	__forceinline Address(uintptr_t a) : m_addr{ a } {}
	__forceinline Address(const void* a) : m_addr{ (uintptr_t)a } {}

	// arithmetic operators for native types.
	__forceinline operator uintptr_t() { 
		return m_addr; 
	}
	__forceinline operator void* () {
		return reinterpret_cast<void*>(m_addr);
	}
	__forceinline operator const void* () { 
		return reinterpret_cast<const void*>(m_addr);
	}

	// is-equals-operator.
	__forceinline bool operator==(Address a) const {
		return as< uintptr_t >() == a.as< uintptr_t >();
	}
	__forceinline bool operator!=(Address a) const {
		return as< uintptr_t >() != a.as< uintptr_t >();
	}

	// cast / add offset and cast.
	template< typename t = Address >
	__forceinline t as() const {
		return (m_addr) ? (t)m_addr : t{};
	}

	template< typename t = Address >
	__forceinline t as(size_t offset) const {
		return (m_addr) ? (t)(m_addr + offset) : t{};
	}

	template< typename t = Address >
	__forceinline t as(ptrdiff_t offset) const {
		return (m_addr) ? (t)(m_addr + offset) : t{};
	}

	// add offset and dereference.
	template< typename t = Address >
	__forceinline t at(size_t offset) const {
		return (m_addr) ? *(t*)(m_addr + offset) : t{};
	}

	template< typename t = Address >
	__forceinline t at(ptrdiff_t offset) const {
		return (m_addr) ? *(t*)(m_addr + offset) : t{};
	}

	// add offset.
	template< typename t = Address >
	__forceinline t add(size_t offset) const {
		return (m_addr) ? (t)(m_addr + offset) : t{};
	}

	template< typename t = Address >
	__forceinline t add(ptrdiff_t offset) const {
		return (m_addr) ? (t)(m_addr + offset) : t{};
	}

	// subtract offset.
	template< typename t = Address >
	__forceinline t sub(size_t offset) const {
		return (m_addr) ? (t)(m_addr - offset) : t{};
	}

	template< typename t = Address >
	__forceinline t sub(ptrdiff_t offset) const {
		return (m_addr) ? (t)(m_addr - offset) : t{};
	}

	// dereference.
	template< typename t = Address >
	__forceinline t to() const {
		return *(t*)m_addr;
	}

	// verify adddress and dereference n times.
	template< typename t = Address >
	__forceinline t get(size_t n = 1) {
		uintptr_t out;

		if (!m_addr)
			return t{};

		out = m_addr;

		for (size_t i{ n }; i > 0; --i) {
			// can't dereference, return null.
			if (!valid(out))
				return t{};

			out = *(uintptr_t*)out;
		}

		return (t)out;
	}


	template< typename t = Address >
	__forceinline t rel32(size_t offset) {
		uintptr_t   out;
		uint32_t    r;

		if (!m_addr)
			return t{};

		out = m_addr + offset;

		// get rel32 offset.
		r = *(uint32_t*)out;
		if (!r)
			return t{};

		// relative to address of next instruction.
		out = (out + 4) + r;

		return (t)out;
	}

	// set.
	template< typename t = uintptr_t > __forceinline void set(const t& value) {
		if (!m_addr)
			return;

		*(t*)m_addr = value;
	}

	// checks if address is not null and has correct page protection.
	static __forceinline bool valid(uintptr_t addr) {
		MEMORY_BASIC_INFORMATION mbi;

		// check for invalid address.
		if (!addr)
			return false;

		// check for invalid page protection.
		if (!VirtualQuery(reinterpret_cast<LPCVOID>(addr), &mbi, sizeof(mbi)))
			return false;

		if (!(mbi.Protect & PAGE_EXECUTE_READWRITE || mbi.Protect & PAGE_EXECUTE_READ))
			return false;

		return true;
	}

	// relative virtual address.
	template< typename t = Address >
	static __forceinline t RVA(Address base, size_t offset) {
		return base.as< t >(offset);
	}
};