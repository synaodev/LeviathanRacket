#ifndef SYNAO_BASIC_RES_HPP
#define SYNAO_BASIC_RES_HPP

#include "./types.hpp"

#define SYNAO_RES_ID_EXTERN_DEF(NAME) extern const char NAME[]

namespace res {
	namespace anim {
		SYNAO_RES_ID_EXTERN_DEF(Austere);
		SYNAO_RES_ID_EXTERN_DEF(Autovator);
		SYNAO_RES_ID_EXTERN_DEF(Barrier);
		SYNAO_RES_ID_EXTERN_DEF(Blast);
		SYNAO_RES_ID_EXTERN_DEF(Bubble);
		SYNAO_RES_ID_EXTERN_DEF(Chest);
		SYNAO_RES_ID_EXTERN_DEF(Computer);
		SYNAO_RES_ID_EXTERN_DEF(DashFlash);
		SYNAO_RES_ID_EXTERN_DEF(Death);
		SYNAO_RES_ID_EXTERN_DEF(Door);
		SYNAO_RES_ID_EXTERN_DEF(Droplet);
		SYNAO_RES_ID_EXTERN_DEF(Fireplace);
		SYNAO_RES_ID_EXTERN_DEF(Foxie);
		SYNAO_RES_ID_EXTERN_DEF(Frontier);
		SYNAO_RES_ID_EXTERN_DEF(GyoShin);
		SYNAO_RES_ID_EXTERN_DEF(Hammer);
		SYNAO_RES_ID_EXTERN_DEF(Heads);
		SYNAO_RES_ID_EXTERN_DEF(Helpful);
		SYNAO_RES_ID_EXTERN_DEF(HolyLance);
		SYNAO_RES_ID_EXTERN_DEF(Items);
		SYNAO_RES_ID_EXTERN_DEF(Kannon);
		SYNAO_RES_ID_EXTERN_DEF(Kyoko);
		SYNAO_RES_ID_EXTERN_DEF(NailRay);
		SYNAO_RES_ID_EXTERN_DEF(Naomi);
		SYNAO_RES_ID_EXTERN_DEF(Nauzika);
		SYNAO_RES_ID_EXTERN_DEF(Senma);
		SYNAO_RES_ID_EXTERN_DEF(Shoshi);
		SYNAO_RES_ID_EXTERN_DEF(Shrapnel);
		SYNAO_RES_ID_EXTERN_DEF(Smoke);
		SYNAO_RES_ID_EXTERN_DEF(Splash);
		SYNAO_RES_ID_EXTERN_DEF(Spring);
		SYNAO_RES_ID_EXTERN_DEF(WolfVulcan);
	}
	namespace sfx {
		SYNAO_RES_ID_EXTERN_DEF(Spark);
		SYNAO_RES_ID_EXTERN_DEF(Bwall);
		SYNAO_RES_ID_EXTERN_DEF(Fan);
		SYNAO_RES_ID_EXTERN_DEF(Blade);
		SYNAO_RES_ID_EXTERN_DEF(Kannon);
		SYNAO_RES_ID_EXTERN_DEF(Explode1);
		SYNAO_RES_ID_EXTERN_DEF(Explode2);
		SYNAO_RES_ID_EXTERN_DEF(Beam);
		SYNAO_RES_ID_EXTERN_DEF(Drill);
		SYNAO_RES_ID_EXTERN_DEF(Spring);
		SYNAO_RES_ID_EXTERN_DEF(Splash);
		SYNAO_RES_ID_EXTERN_DEF(Text);
		SYNAO_RES_ID_EXTERN_DEF(Select);
		SYNAO_RES_ID_EXTERN_DEF(TitleBeg);
		SYNAO_RES_ID_EXTERN_DEF(Inven);
		SYNAO_RES_ID_EXTERN_DEF(BrokenBarrier);
		SYNAO_RES_ID_EXTERN_DEF(Damage);
		SYNAO_RES_ID_EXTERN_DEF(Walk);
		SYNAO_RES_ID_EXTERN_DEF(Landing);
		SYNAO_RES_ID_EXTERN_DEF(Jump);
		SYNAO_RES_ID_EXTERN_DEF(Projectile0);
		SYNAO_RES_ID_EXTERN_DEF(Razor);
	}
	namespace img {
		SYNAO_RES_ID_EXTERN_DEF(Heads);
	}
	namespace pal {
		SYNAO_RES_ID_EXTERN_DEF(Heads);
	}
	namespace icon {
		sint_t width();
		sint_t height();
		sint_t depth();
		sint_t pitch();
		optr_t pixels(); 
	}
}

#endif // SYNAO_BASIC_RES_HPP