// Unit tests for typechecking new Checked C function pointers
//
// The following line is for the LLVM test harness:
// RUN: %clang_cc1 -fcheckedc-extension -verify %s
//

#include "../../include/stdchecked.h"

int f0(int a) {
  return a;
}
float g0(float a) {
  return a;
}
int (*bad_f0)(int) = (int(*)(int))0xdeadbeef;

void takes_safe(_Ptr<int(int)> fun) {
  return;
}

// Nulls are specifically allowed
ptr<int(int)> allowed_convert_null0 = 0;
ptr<int(int)> allowed_convert_null1 = (int(*)(int))0;
ptr<int(int)> allowed_convert_null3 = *(int(*)(int))0;
ptr<int(int)> allowed_convert_null4 = &*(int(*)(int))0;
ptr<int(int)> allowed_convert_null6 = ***(int(*)(int))0;

// Named Functions are specifically allowed
ptr<int(int)> allowed_convert1 = f0;
// Pointers to named functions are specifically allowed
ptr<int(int)> allowed_convert2 = &f0;
ptr<int(int)> allowed_convert3 = *f0;
ptr<int(int)> allowed_convert4 = &*f0;
ptr<int(int)> allowed_convert5 = *&f0;
ptr<int(int)> allowed_convert6 = ***f0;

// Arbitrary Data is definitely not allowed
ptr<int(int)> bad_convert1 = (int(*)(int))0xdeadbeef; // expected-error {{can only cast function names or null pointers to checked function pointer type '_Ptr<int (int)>'}}
ptr<int(int)> bad_convert2 = (int(*)(int))g0;         // expected-error {{cast to checked function pointer type '_Ptr<int (int)>' from incompatible type 'float (float)'}}

// Now locally within a function body
void local_convert(int(*f1)(int), ptr<int(int)> f2) {

  //
  // Casts at assignment-time
  //

  ptr<int(int)> local_safe = 0;

  // Nulls are specifically allowed
  ptr<int(int)> safe_null0 = 0;
  ptr<int(int)> safe_null1 = (int(*)(int))0;
  ptr<int(int)> safe_null3 = *(int(*)(int))0;
  ptr<int(int)> safe_null4 = &*(int(*)(int))0;
  ptr<int(int)> safe_null6 = ***(int(*)(int))0;

  // Top-level declared functions are allowed
  ptr<int(int)> safe_f1 = f0;
  ptr<int(int)> safe_f2 = &f0;
  ptr<int(int)> safe_f3 = *f0;
  ptr<int(int)> safe_f4 = &*f0;
  ptr<int(int)> safe_f5 = *&f0;
  ptr<int(int)> safe_f6 = ***f0;

  // Top-level declared functions are allowed
  ptr<int(int)> safe_f7 = f2;
  ptr<int(int)> safe_f8 = *f2;
  ptr<int(int)> safe_f9 = &*f2;
  ptr<int(int)> safe_f10 = *&f2;
  ptr<int(int)> safe_f11 = ***f2;

  // Top-level declared functions are allowed
  ptr<int(int)> safe_f12 = local_safe;
  ptr<int(int)> safe_f13 = *local_safe;
  ptr<int(int)> safe_f14 = &*local_safe;
  ptr<int(int)> safe_f15 = *&local_safe;
  ptr<int(int)> safe_f16 = ***local_safe;

  // Parameter functions are not allowed
  ptr<int(int)> unsafe_f1 = f1;    // expected-error {{cannot guarantee operand of cast to checked function pointer type '_Ptr<int (int)>' is a function pointer}}
  ptr<int(int)> unsafe_f3 = *f1;   // expected-error {{cannot guarantee operand of cast to checked function pointer type '_Ptr<int (int)>' is a function pointer}}
  ptr<int(int)> unsafe_f4 = &*f1;  // expected-error {{cannot guarantee operand of cast to checked function pointer type '_Ptr<int (int)>' is a function pointer}}
  ptr<int(int)> unsafe_f5 = *&f1;  // expected-error {{cannot guarantee operand of cast to checked function pointer type '_Ptr<int (int)>' is a function pointer}}
  ptr<int(int)> unsafe_f6 = ***f1; // expected-error {{cannot guarantee operand of cast to checked function pointer type '_Ptr<int (int)>' is a function pointer}}

  // Arbitary data is definitely not allowed
  ptr<int(int)> bad_convert1 = (int(*)(int))0xdeadbeef; // expected-error {{can only cast function names or null pointers to checked function pointer type '_Ptr<int (int)>'}}
  ptr<int(int)> bad_convert2 = bad_f0;                  // expected-error {{cannot guarantee operand of cast to checked function pointer type '_Ptr<int (int)>' is a function pointer}}
  ptr<int(int)> bad_convert3 = *bad_f0;                 // expected-error {{cannot guarantee operand of cast to checked function pointer type '_Ptr<int (int)>' is a function pointer}}
  ptr<int(int)> bad_convert4 = ***bad_f0;               // expected-error {{cannot guarantee operand of cast to checked function pointer type '_Ptr<int (int)>' is a function pointer}}

  //
  // Casts in a Call
  //

  // Nulls are specifically allowed
  takes_safe(0);
  takes_safe((int(*)(int))0);
  takes_safe(*(int(*)(int))0);
  takes_safe(&*(int(*)(int))0);
  takes_safe(***(int(*)(int))0);

  // Top-level declared functions are allowed
  takes_safe(f0);
  takes_safe(*f0);
  takes_safe(&*f0);
  takes_safe(*&f0);
  takes_safe(***f0);

  // Local checked functions are allowed
  takes_safe(local_safe);
  takes_safe(*local_safe);
  takes_safe(&*local_safe);
  takes_safe(*&local_safe);
  takes_safe(***local_safe);

  // Unchecked parameter functions are not allowed
  takes_safe(f1);    // expected-error {{cannot guarantee operand of cast to checked function pointer type '_Ptr<int (int)>' is a function pointer}}
  takes_safe(*f1);   // expected-error {{cannot guarantee operand of cast to checked function pointer type '_Ptr<int (int)>' is a function pointer}}
  takes_safe(&*f1);  // expected-error {{cannot guarantee operand of cast to checked function pointer type '_Ptr<int (int)>' is a function pointer}}
  takes_safe(*&f1);  // expected-error {{cannot guarantee operand of cast to checked function pointer type '_Ptr<int (int)>' is a function pointer}}
  takes_safe(***f1); // expected-error {{cannot guarantee operand of cast to checked function pointer type '_Ptr<int (int)>' is a function pointer}}

  // Arbitrary data is definitely not allowed
  takes_safe((int(*)(int))0xdeadbeef); // expected-error {{can only cast function names or null pointers to checked function pointer type '_Ptr<int (int)>'}}
  takes_safe(bad_f0);                  // expected-error {{cannot guarantee operand of cast to checked function pointer type '_Ptr<int (int)>' is a function pointer}}
  takes_safe(*bad_f0);                 // expected-error {{cannot guarantee operand of cast to checked function pointer type '_Ptr<int (int)>' is a function pointer}}
  takes_safe(***bad_f0);               // expected-error {{cannot guarantee operand of cast to checked function pointer type '_Ptr<int (int)>' is a function pointer}}

  //
  // Explicit User Casts
  //

  int(*local_unsafe)(int) = f0; // This is a valid unchecked to unchecked assignment

  ptr<int(int)> safe_cast_null0 = (ptr<int(int)>)0;
  ptr<int(int)> safe_cast_null1 = (ptr<int(int)>)((int(*)(int))0);
  ptr<int(int)> safe_cast_null3 = (ptr<int(int)>)(*(int(*)(int))0);
  ptr<int(int)> safe_cast_null4 = (ptr<int(int)>)(&*(int(*)(int))0);
  ptr<int(int)> safe_cast_null6 = (ptr<int(int)>)(***(int(*)(int))0);

  // Top-level declared functions are allowed
  ptr<int(int)> safe_cast_f1 = (ptr<int(int)>)(f0);
  ptr<int(int)> safe_cast_f2 = (ptr<int(int)>)(&f0);
  ptr<int(int)> safe_cast_f3 = (ptr<int(int)>)(*f0);
  ptr<int(int)> safe_cast_f4 = (ptr<int(int)>)(&*f0);
  ptr<int(int)> safe_cast_f5 = (ptr<int(int)>)(*&f0);
  ptr<int(int)> safe_cast_f6 = (ptr<int(int)>)(***f0);

  // Checked Parameters are allowed
  ptr<int(int)> safe_cast_f7 = (ptr<int(int)>)(f2);
  ptr<int(int)> safe_cast_f8 = (ptr<int(int)>)(*f2);
  ptr<int(int)> safe_cast_f9 = (ptr<int(int)>)(&*f2);
  ptr<int(int)> safe_cast_f10 = (ptr<int(int)>)(*&f2);
  ptr<int(int)> safe_cast_f11 = (ptr<int(int)>)(***f2);

  // Top-level declared functions are allowed
  ptr<int(int)> safe_cast_f12 = (ptr<int(int)>)(local_safe);
  ptr<int(int)> safe_cast_f13 = (ptr<int(int)>)(*local_safe);
  ptr<int(int)> safe_cast_f14 = (ptr<int(int)>)(&*local_safe);
  ptr<int(int)> safe_cast_f15 = (ptr<int(int)>)(*&local_safe);
  ptr<int(int)> safe_cast_f16 = (ptr<int(int)>)(***local_safe);

  // Parameter functions are not allowed
  ptr<int(int)> unsafe_cast_f1 = (ptr<int(int)>)(f1);    // expected-error {{cannot guarantee operand of cast to checked function pointer type '_Ptr<int (int)>' is a function pointer}}
  ptr<int(int)> unsafe_cast_f3 = (ptr<int(int)>)(*f1);   // expected-error {{cannot guarantee operand of cast to checked function pointer type '_Ptr<int (int)>' is a function pointer}}
  ptr<int(int)> unsafe_cast_f4 = (ptr<int(int)>)(&*f1);  // expected-error {{cannot guarantee operand of cast to checked function pointer type '_Ptr<int (int)>' is a function pointer}}
  ptr<int(int)> unsafe_cast_f5 = (ptr<int(int)>)(*&f1);  // expected-error {{cannot guarantee operand of cast to checked function pointer type '_Ptr<int (int)>' is a function pointer}}
  ptr<int(int)> unsafe_cast_f6 = (ptr<int(int)>)(***f1); // expected-error {{cannot guarantee operand of cast to checked function pointer type '_Ptr<int (int)>' is a function pointer}}

  // Arbitary data is definitely not allowed
  ptr<int(int)> bad_cast_1 = (ptr<int(int)>)((int(*)(int))0xdeadbeef); // expected-error {{can only cast function names or null pointers to checked function pointer type '_Ptr<int (int)>'}}
  ptr<int(int)> bad_cast_2 = (ptr<int(int)>)(bad_f0);                  // expected-error {{cannot guarantee operand of cast to checked function pointer type '_Ptr<int (int)>' is a function pointer}}
  ptr<int(int)> bad_cast_3 = (ptr<int(int)>)(*bad_f0);                 // expected-error {{cannot guarantee operand of cast to checked function pointer type '_Ptr<int (int)>' is a function pointer}}
  ptr<int(int)> bad_cast_4 = (ptr<int(int)>)(***bad_f0);               // expected-error {{cannot guarantee operand of cast to checked function pointer type '_Ptr<int (int)>' is a function pointer}}

  //
  // Weird Casts
  //

  ptr<float(float, int)> local_safe2 = 0;
  int(*local_odd)(int) = local_unsafe;
  ptr<int(int)> local_super_unsafe1 = local_unsafe;                // expected-error {{cannot guarantee operand of cast to checked function pointer type '_Ptr<int (int)>' is a function pointer}}
  ptr<int(int)> local_super_unsafe2 = (ptr<int(int)>)local_unsafe; // expected-error {{cannot guarantee operand of cast to checked function pointer type '_Ptr<int (int)>' is a function pointer}}

  //
  // Weird Unary Operators
  //

  // There are more tests in the platform-specific tests

  int(**local_more_unsafe1)(int) = &local_unsafe;
  ptr<int(int)> local_more_unsafe2 = *local_more_unsafe1; // expected-error {{cannot guarantee operand of cast to checked function pointer type '_Ptr<int (int)>' is a function pointer}}

  int local_not_fn;
  ptr<int(int)> local_not_fn2 = (int(*)(int))&local_not_fn; // expected-error {{cast to checked function pointer type '_Ptr<int (int)>' may only take the address of expressions with function type}}

}

struct S1 {
  ptr<int(int, int)> f1;
  int(*f2)(int, int);
};

union U1 {
  ptr<int(int, int)> f1;
  int(*f2)(int, int);
};

void field_access(struct S1 s1, union U1 u1) {
  ptr<int(int, int)> safe1 = s1.f1;
  ptr<int(int, int)> unsafe1 = s1.f2; // expected-error {{cannot guarantee operand of cast to checked function pointer type '_Ptr<int (int, int)>' is a function pointer}}

  // safe2 should probably not be allowed, but it is.
  ptr<int(int, int)> safe2 = u1.f1;
  ptr<int(int, int)> unsafe2 = u1.f2; // expected-error {{cannot guarantee operand of cast to checked function pointer type '_Ptr<int (int, int)>' is a function pointer}}
}