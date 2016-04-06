/*
* <LuaMath.cpp>
*
* This file is a part of Metacade.
* Copyright (C) 2016 Zachary Blystone. See LICENSE for more details
*/

#include "LuaInput.h"
#include "Runtime.h"
#include <cmath>
#include <cstring>

using namespace Arcade;

const float RAD_2_DEG = 57.29577f;
const float DEG_2_RAD = 1.f / RAD_2_DEG;

class LuaVec2Module : public LuaModule
{
public:
	float X, Y;
	static LuaVec2Module C;

	virtual void __get(lua_State *L) 
	{
		getTable(L);

		lua_getfield(L, -1, "x"); X = lua_tonumber(L, -1); lua_pop(L, 1);
		lua_getfield(L, -1, "y"); Y = lua_tonumber(L, -1); lua_pop(L, 1);
	}

	virtual void __set(lua_State *L) 
	{
		getTable(L);
		lua_pushnumber(L, X); lua_setfield(L, -2, "x");
		lua_pushnumber(L, Y); lua_setfield(L, -2, "y");
	}

	MODULE_FUNCTION_DEF(__add)
	{
		LuaVec2Module *V1 = GET_OBJECT(LuaVec2Module, L, 1);
		LuaVec2Module *V2 = GET_OBJECT(LuaVec2Module, L, 2);
		if (!V1 || !V2) return 0;

		LuaVec2Module *O = (LuaVec2Module*)C.push(L);
		if (!O) return 0;

		O->X = V1->X - V2->X;
		O->Y = V1->Y - V2->Y;
		return 1;
	}

	MODULE_FUNCTION_DEF(__sub)
	{
		LuaVec2Module *V1 = GET_OBJECT(LuaVec2Module, L, 1);
		LuaVec2Module *V2 = GET_OBJECT(LuaVec2Module, L, 2);
		if (!V1 || !V2) return 0;

		LuaVec2Module *O = (LuaVec2Module*) C.push(L);
		if (!O) return 0;

		O->X = V1->X - V2->X;
		O->Y = V1->Y - V2->Y;
		return 1;
	}

	MODULE_FUNCTION_DEF(__mul)
	{
		LuaVec2Module *V1 = GET_OBJECT(LuaVec2Module, L, 1);
		if (!V1) return 0;

		if (lua_type(L, 2) == LUA_TUSERDATA)
		{
			LuaVec2Module *V2 = GET_OBJECT(LuaVec2Module, L, 2);
			if (!V2) return 0;

			C.X = V1->X * V2->X; 
			C.Y = V1->Y * V2->Y;
			C.push(L);
			return 1;
		}
		else if (lua_type(L, 2) == LUA_TNUMBER)
		{
			float N = luaL_checknumber(L, 2);
			C.X = V1->X * N;
			C.Y = V1->Y * N;
			C.push(L);
			return 1;
		}
		return 0;
	}

	MODULE_FUNCTION_DEF(__div)
	{
		LuaVec2Module *V1 = GET_OBJECT(LuaVec2Module, L, 1);
		if (!V1) return 0;

		if (lua_type(L, 2) == LUA_TUSERDATA)
		{
			LuaVec2Module *V2 = GET_OBJECT(LuaVec2Module, L, 2);
			if (!V2) return 0;

			C.X = V1->X * V2->X;
			C.Y = V1->Y * V2->Y;
			C.push(L);
			return 1;
		}
		else if (lua_type(L, 2) == LUA_TNUMBER)
		{
			float N = luaL_checknumber(L, 2);
			C.X = V1->X * N;
			C.Y = V1->Y * N;
			C.push(L);
			return 1;
		}
		return 0;
	}

	MODULE_FUNCTION_DEF(__unm)
	{
		LuaVec2Module *V = GET_OBJECT(LuaVec2Module, L, 1);
		if (!V) return 0;

		C.X = -V->X;
		C.Y = -V->Y;
		C.push(L);
		return 1;
	}

	MODULE_FUNCTION_DEF(__tostring)
	{
		LuaVec2Module *V = GET_OBJECT(LuaVec2Module, L, 1);
		if (!V) return 0;

		lua_pushfstring(L, "[%f, %f]", V->X, V->Y);
		return 1;
	}


	MODULE_FUNCTION_DEF(Length)
	{
		LuaVec2Module *V = GET_OBJECT(LuaVec2Module, L, 1);
		if (!V) return 0;

		lua_pushnumber(L, sqrtf(V->X * V->X + V->Y * V->Y));
		return 1;
	}

	MODULE_FUNCTION_DEF(LengthSquared)
	{
		LuaVec2Module *V = GET_OBJECT(LuaVec2Module, L, 1);
		if (!V) return 0;

		lua_pushnumber(L, V->X * V->X + V->Y * V->Y);
		return 1;
	}

	MODULE_FUNCTION_DEF(Cross)
	{
		LuaVec2Module *V1 = GET_OBJECT(LuaVec2Module, L, 1);
		LuaVec2Module *V2 = GET_OBJECT(LuaVec2Module, L, 2);
		if (!V1 || !V2) return 0;

		lua_pushnumber(L, V1->X * V2->Y - V1->Y * V2->X);
		return 1;
	}

	MODULE_FUNCTION_DEF(Dot)
	{
		LuaVec2Module *V1 = GET_OBJECT(LuaVec2Module, L, 1);
		LuaVec2Module *V2 = GET_OBJECT(LuaVec2Module, L, 2);
		if (!V1 || !V2) return 0;

		lua_pushnumber(L, V1->X * V2->X + V1->Y * V2->Y);
		return 1;
	}

	static int constructor(lua_State *L)
	{
		C.X = luaL_optnumber(L, 1, 0.f);
		C.Y = luaL_optnumber(L, 2, 0.f);

		LuaVec2Module *O = (LuaVec2Module*)C.push(L);
		if (!O) return 0;

		return 1;
	}

	DEFINE_MODULE(LuaVec2Module, Vector)
	MODULE_CONSTRUCTOR(constructor)
	MODULE_FUNCTION(__add)
	MODULE_FUNCTION(__sub)
	MODULE_FUNCTION(__mul)
	MODULE_FUNCTION(__div)
	MODULE_FUNCTION(__unm)
	MODULE_FUNCTION(__tostring)
	MODULE_FUNCTION(Length)
	MODULE_FUNCTION(LengthSquared)
	MODULE_FUNCTION(Cross)
	MODULE_FUNCTION(Dot)
	END_DEFINE_MODULE()
}; CREATE_MODULE(LuaVec2Module);

LuaVec2Module LuaVec2Module::C;

class LuaMatrix3x3Module : public LuaModule
{
public:
	float Mtx[9];
	static LuaMatrix3x3Module C;

	void MakeIdentity()
	{
		static const float Identity[] = {
			1.f, 0.f, 0.f,
			0.f, 1.f, 0.f,
			0.f, 0.f, 1.f
		};
		Set(Identity);
	}

	static void MatrixMultiply(float a[9], float b[9], float c[9])
	{
		c[0] = a[0] * b[0] + a[1] * b[3] + a[2] * b[6];
		c[1] = a[0] * b[1] + a[1] * b[4] + a[2] * b[7];
		c[2] = a[0] * b[2] + a[1] * b[5] + a[2] * b[8];

		c[3] = a[3] * b[0] + a[4] * b[3] + a[5] * b[6];
		c[4] = a[3] * b[1] + a[4] * b[4] + a[5] * b[7];
		c[5] = a[3] * b[2] + a[4] * b[5] + a[5] * b[8];

		c[6] = a[6] * b[0] + a[7] * b[3] + a[8] * b[6];
		c[7] = a[6] * b[1] + a[7] * b[4] + a[8] * b[7];
		c[8] = a[6] * b[2] + a[7] * b[5] + a[8] * b[8];
	}

	void Set(const float Other[9])
	{
		memcpy(Mtx, Other, sizeof(float) * 9);
	}

	void MultiplyWith(LuaMatrix3x3Module *Other, LuaMatrix3x3Module *Out)
	{
		MatrixMultiply(this->Mtx, Other->Mtx, Out->Mtx);
	}

	void ComposeWith(LuaMatrix3x3Module *Other, bool Pre)
	{
		static float Swap[9];
		MatrixMultiply(Pre ? this->Mtx : Other->Mtx, Pre ? Other->Mtx : this->Mtx, Swap);
		Set(Swap);
	}

	static int constructor(lua_State *L)
	{
		LuaMatrix3x3Module *O = (LuaMatrix3x3Module* ) C.push(L);
		if (!O) return 0;

		Runtime::LogPrint(LOG_MESSAGE, "UH OK?");

		O->MakeIdentity();

		return 1;
	}

	MODULE_FUNCTION_DEF(Translate)
	{
		LuaMatrix3x3Module *M = GET_OBJECT(LuaMatrix3x3Module, L, 1);
		if (!M) return 0;

		float X, Y;
		if (lua_type(L, 2) == LUA_TUSERDATA)
		{
			LuaVec2Module *V = GET_OBJECT(LuaVec2Module, L, 2);
			if (!V) return 0;
			X = V->X;
			Y = V->Y;
		}
		else if (lua_type(L, 2) == LUA_TNUMBER)
		{
			X = luaL_checknumber(L, 2);
			Y = luaL_checknumber(L, 3);
		}
		else { return 0; }

		static float TX[] = {
			1.f, 0.f, X,
			0.f, 1.f, Y,
			0.f, 0.f, 1.f
		};

		static float Result[9];
		MatrixMultiply(TX, M->Mtx, Result);
		M->Set(Result);

		lua_pushvalue(L, 1);
		return 1;
	}

	MODULE_FUNCTION_DEF(Rotate)
	{
		LuaMatrix3x3Module *M = GET_OBJECT(LuaMatrix3x3Module, L, 1);
		float R = luaL_checknumber(L, 2) * DEG_2_RAD;
		if (!M) return 0;

		float S = sinf(R);
		float C = cosf(R);

		static float TX[] = {
			C, -S, 0.f,
			S, C, 0.f,
			0.f, 0.f, 1.f
		};

		static float Result[9];
		MatrixMultiply(TX, M->Mtx, Result);
		M->Set(Result);

		lua_pushvalue(L, 1);
		return 1;
	}

	MODULE_FUNCTION_DEF(Scale)
	{
		LuaMatrix3x3Module *M = GET_OBJECT(LuaMatrix3x3Module, L, 1);
		if (!M) return 0;

		float X, Y;
		if (lua_type(L, 2) == LUA_TUSERDATA)
		{
			LuaVec2Module *V = GET_OBJECT(LuaVec2Module, L, 2);
			if (!V) return 0;
			X = V->X;
			Y = V->Y;
		}
		else if (lua_type(L, 2) == LUA_TNUMBER)
		{
			X = luaL_checknumber(L, 2);
			Y = luaL_optnumber(L, 3, X);
		}
		else { return 0; }

		static float TX[] = {
			X, 0.f, 0.f,
			0.f, Y, 0.f,
			0.f, 0.f, 1.f
		};

		static float Result[9];
		MatrixMultiply(TX, M->Mtx, Result);
		M->Set(Result);

		lua_pushvalue(L, 1);
		return 1;
	}

	MODULE_FUNCTION_DEF(Compose)
	{
		LuaMatrix3x3Module *M1 = GET_OBJECT(LuaMatrix3x3Module, L, 1);
		LuaMatrix3x3Module *M2 = GET_OBJECT(LuaMatrix3x3Module, L, 2);
		if (!M1 || !M2) return 0;

		bool Pre = lua_isboolean(L, 3) && lua_toboolean(L, 3);
		M1->ComposeWith(M2, Pre);

		lua_pushvalue(L, 1);
		return 1;
	}

	MODULE_FUNCTION_DEF(Transform)
	{
		LuaMatrix3x3Module *M = GET_OBJECT(LuaMatrix3x3Module, L, 1);
		LuaVec2Module *V = GET_OBJECT(LuaVec2Module, L, 2);
		if (!M || !V) return 0;

		LuaVec2Module *O = PUSH_MODULE(LuaVec2Module, L);
		if (!O) return 0;

		O->X = V->X * M->Mtx[0] + V->Y * M->Mtx[1] + M->Mtx[2];
		O->Y = V->X * M->Mtx[3] + V->Y * M->Mtx[4] + M->Mtx[5];

		lua_pushvalue(L, 2);
		return 1;
	}

	MODULE_FUNCTION_DEF(__mul)
	{
		LuaMatrix3x3Module *M1 = GET_OBJECT(LuaMatrix3x3Module, L, 1);
		LuaMatrix3x3Module *M2 = GET_OBJECT(LuaMatrix3x3Module, L, 2);
		if (!M1 || !M2) return 0;

		LuaMatrix3x3Module *O = (LuaMatrix3x3Module *) C.push(L);
		if (!O) return 0;

		M1->MultiplyWith(M2, O);

		return 1;
	}

	MODULE_FUNCTION_DEF(__tostring)
	{
		LuaMatrix3x3Module *M = GET_OBJECT(LuaMatrix3x3Module, L, 1);
		if (!M) return 0;

		float *TX = M->Mtx;

		lua_pushfstring(L, "[%f, %f, %f],\n[%f, %f, %f],\n[%f, %f, %f]",
			TX[0], TX[1], TX[2],
			TX[3], TX[4], TX[5],
			TX[6], TX[7], TX[8]);
		return 1;
	}

	DEFINE_MODULE(LuaMatrix3x3Module, XForm)
	MODULE_CONSTRUCTOR(constructor)
	MODULE_FUNCTION(Translate)
	MODULE_FUNCTION(Rotate)
	MODULE_FUNCTION(Scale)
	MODULE_FUNCTION(Compose)
	MODULE_FUNCTION(Transform)
	MODULE_FUNCTION(__mul)
	MODULE_FUNCTION(__tostring)
	END_DEFINE_MODULE()
}; CREATE_MODULE(LuaMatrix3x3Module);

LuaMatrix3x3Module LuaMatrix3x3Module::C;

void OpenLuaMathModule(lua_State *L)
{
	OPEN_MODULE(L, LuaVec2Module);
	OPEN_MODULE(L, LuaMatrix3x3Module);
}

bool lua_toVec2(lua_State *L, int idx, float &x, float &y)
{
	LuaVec2Module *V = GET_OBJECT(LuaVec2Module, L, idx);
	if (!V) return false;
	
	x = V->X;
	y = V->Y;

	return true;
}

void lua_pushVec2(lua_State *L, float x, float y)
{
	LuaVec2Module *V = PUSH_MODULE(LuaVec2Module, L);
	if (V)
	{
		V->X = x;
		V->Y = y;
	}
}

float * lua_toMatrix3x3(lua_State *L, int idx)
{
	LuaMatrix3x3Module *M = GET_OBJECT(LuaMatrix3x3Module, L, idx);
	if (M)
	{
		return M->Mtx;
	}
	return nullptr;
}

void lua_pushMatrix3x3(lua_State *L, float *Mtx)
{
	LuaMatrix3x3Module *V = PUSH_MODULE(LuaMatrix3x3Module, L);
	if (V)
	{
		V->Set(Mtx);
	}
}
