/* NetHack 3.6	u_init.c	$NHDT-Date: 1575245094 2019/12/02 00:04:54 $  $NHDT-Branch: NetHack-3.6 $:$NHDT-Revision: 1.60 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/*-Copyright (c) Robert Patrick Rankin, 2017. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

struct trobj {
    short trotyp;         // object
    schar trspe;          // enchantment
    char trclass;         // type of object
    Bitfield(trquan, 6);  // quantity
    Bitfield(trbless, 2); // BUC - blessed uncursed cursed
};

STATIC_DCL void FDECL(ini_inv, (struct trobj *));
STATIC_DCL void FDECL(knows_object, (int));
STATIC_DCL void FDECL(knows_class, (CHAR_P));
STATIC_DCL boolean FDECL(restricted_spell_discipline, (int));

#define UNDEF_TYP 0
#define UNDEF_SPE '\177'
#define UNDEF_BLESS 2

/*
 *      Initial inventory for the various roles.
 */

static struct trobj Archeologist[] = {
    /* if adventure has a name...  idea from tan@uvm-gen */
    { BULLWHIP, 2, WEAPON_CLASS, 1, UNDEF_BLESS },
    { LEATHER_JACKET, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { FEDORA, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { FOOD_RATION, 0, FOOD_CLASS, 3, 0 },
    { PICK_AXE, UNDEF_SPE, TOOL_CLASS, 1, UNDEF_BLESS },
    { TINNING_KIT, UNDEF_SPE, TOOL_CLASS, 1, UNDEF_BLESS },
    { TOUCHSTONE, 0, GEM_CLASS, 1, 0 },
    { SACK, 0, TOOL_CLASS, 1, 0 },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Barbarian[] = {
#define B_MAJOR 0 /* two-handed sword or battle-axe  */
#define B_MINOR 1 /* matched with axe or short sword */
    { TWO_HANDED_SWORD, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
    { AXE, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
    { RING_MAIL, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { FOOD_RATION, 0, FOOD_CLASS, 1, 0 },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Beast_tamer[] = {
    { BULLWHIP, 3, WEAPON_CLASS, 1, UNDEF_BLESS },
    { BULLWHIP, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
    { KNIFE, 0, WEAPON_CLASS, 3, UNDEF_BLESS },
    { HIGH_BOOTS, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { LEATHER_GLOVES, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { FOOD_RATION, 0, FOOD_CLASS, 1, 0 },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Cave_man[] = {
#define C_AMMO 2
    { CLUB, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
    { SLING, 2, WEAPON_CLASS, 1, UNDEF_BLESS },
    { FLINT, 0, GEM_CLASS, 15, UNDEF_BLESS }, /* quan is variable */
    { ROCK, 0, GEM_CLASS, 3, 0 },             /* yields 18..33 */
    { LEATHER_ARMOR, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Dragon_rider[] = {
#define D_POLEARM 0
	{ UNDEF_TYP, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
    { KNIFE, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
    { LEATHER_ARMOR, 1, ARMOR_CLASS, 1, UNDEF_BLESS },
    { HIGH_BOOTS, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { LEATHER_GLOVES, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ FOOD_RATION, 0, FOOD_CLASS, 1, 0},
    { TRIPE_RATION, 0, FOOD_CLASS, 5, 0 },
    { SADDLE, 0, TOOL_CLASS, 1, 0 },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Flame_Mage[] = {
#define F_BOOK 9
    { QUARTERSTAFF, 1, WEAPON_CLASS, 1, 1 },
    { STUDDED_LEATHER_ARMOR, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { FOOD_RATION, 0, FOOD_CLASS, 2, 0 },
    { UNDEF_TYP, UNDEF_SPE, POTION_CLASS, 1, UNDEF_BLESS },
    { UNDEF_TYP, UNDEF_SPE, SCROLL_CLASS, 1, UNDEF_BLESS },
    { WAN_FIRE, UNDEF_SPE, WAND_CLASS, 1, UNDEF_BLESS },
    { UNDEF_TYP, UNDEF_SPE, RING_CLASS, 1, UNDEF_BLESS },
    { SPE_FLAME_SPHERE, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
    { SPE_FIREBALL, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
    { UNDEF_TYP, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Gunslinger[] = {
#define G_BULLETS 2
    { PISTOL, 1, WEAPON_CLASS, 1, 0 },
	{ KNIFE, 0, WEAPON_CLASS, 1, 0},
    { BULLET, 0, WEAPON_CLASS, 20, 0 },
    { FEDORA, 0, ARMOR_CLASS, 1, 0 },
    { LEATHER_JACKET, 0, ARMOR_CLASS, 1, 0 },
    { HIGH_BOOTS, 0, ARMOR_CLASS, 1, 0 },
    { TIN, 0, FOOD_CLASS, 5, 0 },
    { POT_BOOZE, 0, POTION_CLASS, 2, 0 },
    { TINNING_KIT, UNDEF_SPE, TOOL_CLASS, 1, UNDEF_BLESS },
    { SACK, 0, TOOL_CLASS, 1, 0 },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Healer[] = {
    { SCALPEL, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
    { LEATHER_GLOVES, 1, ARMOR_CLASS, 1, UNDEF_BLESS },
    { STETHOSCOPE, 0, TOOL_CLASS, 1, 0 },
    { POT_HEALING, 0, POTION_CLASS, 4, UNDEF_BLESS },
    { POT_EXTRA_HEALING, 0, POTION_CLASS, 4, UNDEF_BLESS },
    { WAN_SLEEP, UNDEF_SPE, WAND_CLASS, 1, UNDEF_BLESS },
    /* always blessed, so it's guaranteed readable */
    { SPE_HEALING, 0, SPBOOK_CLASS, 1, 1 },
    { SPE_EXTRA_HEALING, 0, SPBOOK_CLASS, 1, 1 },
    { SPE_STONE_TO_FLESH, 0, SPBOOK_CLASS, 1, 1 },
    { APPLE, 0, FOOD_CLASS, 5, 0 },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Ice_Mage[] = {
#define I_BOOK 9
    { QUARTERSTAFF, 1, WEAPON_CLASS, 1, 1 },
    { STUDDED_LEATHER_ARMOR, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { FOOD_RATION, 0, FOOD_CLASS, 2, 0 },
    { UNDEF_TYP, UNDEF_SPE, POTION_CLASS, 1, UNDEF_BLESS },
    { UNDEF_TYP, UNDEF_SPE, SCROLL_CLASS, 1, UNDEF_BLESS },
    { WAN_COLD, UNDEF_SPE, WAND_CLASS, 1, UNDEF_BLESS },
    { UNDEF_TYP, UNDEF_SPE, RING_CLASS, 1, UNDEF_BLESS },
    { SPE_FREEZE_SPHERE, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
    { SPE_CONE_OF_COLD, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
    { UNDEF_TYP, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Knight[] = {
    { LONG_SWORD, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
    { LANCE, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
    { RING_MAIL, 1, ARMOR_CLASS, 1, UNDEF_BLESS },
    { HELMET, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { SMALL_SHIELD, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { LEATHER_GLOVES, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { APPLE, 0, FOOD_CLASS, 10, 0 },
    { CARROT, 0, FOOD_CLASS, 10, 0 },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Monk[] = {
#define M_BOOK 2
    { LEATHER_GLOVES, 2, ARMOR_CLASS, 1, UNDEF_BLESS },
    { ROBE, 1, ARMOR_CLASS, 1, UNDEF_BLESS },
    { UNDEF_TYP, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
    { UNDEF_TYP, UNDEF_SPE, SCROLL_CLASS, 1, UNDEF_BLESS },
    { POT_HEALING, 0, POTION_CLASS, 3, UNDEF_BLESS },
    { FOOD_RATION, 0, FOOD_CLASS, 3, 0 },
    { APPLE, 0, FOOD_CLASS, 5, UNDEF_BLESS },
    { ORANGE, 0, FOOD_CLASS, 5, UNDEF_BLESS },
    /* Yes, we know fortune cookies aren't really from China.  They were
     * invented by George Jung in Los Angeles, California, USA in 1916.
     */
    { FORTUNE_COOKIE, 0, FOOD_CLASS, 3, UNDEF_BLESS },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Naked[] = {
	{ BLINDFOLD, 0, TOOL_CLASS, 1, 0},
    { 0, 0, 0, 0, 0 }
};
static struct trobj Necromancer[] = {
#define N_BOOK 6
    { QUARTERSTAFF, 1, WEAPON_CLASS, 1, 1 },
    { UNDEF_TYP, UNDEF_SPE, RING_CLASS, 2, UNDEF_BLESS },
    { UNDEF_TYP, UNDEF_SPE, POTION_CLASS, 3, UNDEF_BLESS },
    { UNDEF_TYP, UNDEF_SPE, SCROLL_CLASS, 3, UNDEF_BLESS },
 //   { SPE_SUMMON_UNDEAD, 0, SPBOOK_CLASS, 1, 1 },
 //   { SPE_COMMAND_UNDEAD, 0, SPBOOK_CLASS, 1, 1 },
    { SPE_DRAIN_LIFE, 0, SPBOOK_CLASS, 1, 1 },
    { WAN_DRAINING, UNDEF_SPE, WAND_CLASS, 1, UNDEF_BLESS },
    { UNDEF_TYP, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
    { PICK_AXE, UNDEF_SPE, TOOL_CLASS, 1, UNDEF_BLESS },
    { POT_GAIN_LEVEL, 0, POTION_CLASS, 10, 0 },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Pirate[] = {
#define P_BULLETS 3
	{ SCIMITAR, 9, WEAPON_CLASS, 1, 0 },
	{ KNIFE, 1, WEAPON_CLASS, 2, 0 },
    { PISTOL, 0, WEAPON_CLASS, 1, 0 },
    { BULLET, 0, WEAPON_CLASS, 20, 0 }, 
	{ FEDORA, 0, ARMOR_CLASS, 1, 0 },
	{ LEATHER_JACKET, 0, ARMOR_CLASS, 1, 0 },
	{ HIGH_BOOTS, 0, ARMOR_CLASS, 1, 0 },
	{ CRAM_RATION, 0, FOOD_CLASS, 2, 0 },
	{ ORANGE, 0, FOOD_CLASS, 3, 0 },
	{ POT_BOOZE, 0, POTION_CLASS, 3, 0 },
    { UNDEF_TYP, UNDEF_SPE, RING_CLASS, 1, UNDEF_BLESS },
	{ OILSKIN_SACK, 0, TOOL_CLASS, 1, 0 },
//	{ WAN_ENLIGHTENMENT, 0, WAND_CLASS, 2, 1 },
//	{ SCR_CHARGING, 0, SCROLL_CLASS, 3, 1 },
    { POT_GAIN_LEVEL, 0, POTION_CLASS, 13, 0 },
	{ POT_FULL_HEALING, 0, POTION_CLASS, 5, 0 },
    { PICK_AXE, 0, TOOL_CLASS, 1, 0 }, 
    { SCR_GOLD_DETECTION, 0, SCROLL_CLASS, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj Priest[] = {
    { MACE, 1, WEAPON_CLASS, 1, 1 },
    { ROBE, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { SMALL_SHIELD, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { POT_WATER, 0, POTION_CLASS, 4, 1 }, /* holy water */
    { CLOVE_OF_GARLIC, 0, FOOD_CLASS, 1, 0 },
    { SPRIG_OF_WOLFSBANE, 0, FOOD_CLASS, 1, 0 },
    { UNDEF_TYP, UNDEF_SPE, SPBOOK_CLASS, 2, UNDEF_BLESS },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Ranger[] = {
#define RAN_BOW 1
#define RAN_TWO_ARROWS 2
#define RAN_ZERO_ARROWS 3
    { DAGGER, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
    { BOW, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
    { ARROW, 2, WEAPON_CLASS, 50, UNDEF_BLESS },
    { ARROW, 0, WEAPON_CLASS, 30, UNDEF_BLESS },
    { CLOAK_OF_DISPLACEMENT, 2, ARMOR_CLASS, 1, UNDEF_BLESS },
    { CRAM_RATION, 0, FOOD_CLASS, 4, 0 },
  //  { WAN_WISHING, 0, WAND_CLASS, 2, 1 },
 //   { SCR_CHARGING, 0, SCROLL_CLASS, 3, 1 },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Rogue[] = {
#define R_DAGGERS 1
    { SHORT_SWORD, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
    { DAGGER, 0, WEAPON_CLASS, 10, 0 }, /* quan is variable */
    { LEATHER_ARMOR, 1, ARMOR_CLASS, 1, UNDEF_BLESS },
    { POT_SICKNESS, 0, POTION_CLASS, 1, 0 },
    { LOCK_PICK, 0, TOOL_CLASS, 1, 0 },
    { SACK, 0, TOOL_CLASS, 1, 0 },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Samurai[] = {
#define S_ARROWS 3
    { KATANA, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
    { SHORT_SWORD, 0, WEAPON_CLASS, 1, UNDEF_BLESS }, /* wakizashi */
    { YUMI, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
    { YA, 0, WEAPON_CLASS, 25, UNDEF_BLESS }, /* variable quan */
    { SPLINT_MAIL, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Shinobi[] = {
#define Shi_SHURIKEN 1
    { SHORT_SWORD, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ SHURIKEN, 2, WEAPON_CLASS, 20, UNDEF_BLESS},
    { CLOAK_OF_DISPLACEMENT, 1, ARMOR_CLASS, 1, UNDEF_BLESS },
    { ELVEN_BOOTS, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { FOOD_RATION, 0, FOOD_CLASS, 2, 0 },
	{ GRAPPLING_HOOK, 0, TOOL_CLASS, 1, 0},
    { 0, 0, 0, 0, 0 }
};
static struct trobj Tourist[] = {
#define T_DARTS 0
    { DART, 2, WEAPON_CLASS, 25, UNDEF_BLESS }, /* quan is variable */
    { UNDEF_TYP, UNDEF_SPE, FOOD_CLASS, 10, 0 },
    { POT_EXTRA_HEALING, 0, POTION_CLASS, 2, UNDEF_BLESS },
    { SCR_MAGIC_MAPPING, 0, SCROLL_CLASS, 4, UNDEF_BLESS },
    { HAWAIIAN_SHIRT, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { EXPENSIVE_CAMERA, UNDEF_SPE, TOOL_CLASS, 1, 0 },
    { CREDIT_CARD, 0, TOOL_CLASS, 1, 0 },
    { 0, 0, 0, 0, 0 }
};
static struct trobj UndeadSlayer[] = {
#define U_MINOR 0 // silver spear or whip [Castlevania] 25/25% 
                  // crossbow 50% [Buffy] 
#define U_RANGE 1 // silver daggers or crossbow bolts 
#define U_MISC 2  // +1 boots [Buffy can kick] or helmet 
#define U_ARMOR 3 // Tshirt/leather +1 or chain mail 
    { WOODEN_STAKE, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
    { SILVER_SPEAR, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
    { SILVER_DAGGER, 0, WEAPON_CLASS, 5, UNDEF_BLESS },
    { HELMET, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { CHAIN_MAIL, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { CLOVE_OF_GARLIC, 0, FOOD_CLASS, 5, 1 },
    { SPRIG_OF_WOLFSBANE, 0, FOOD_CLASS, 5, 1 },
   // { HOLY_WAFER, 0, FOOD_CLASS, 4, 0 },
    { POT_WATER, 0, POTION_CLASS, 4, 1 }, 
    { 0, 0, 0, 0, 0 }
};
static struct trobj Valkyrie[] = {
    { LONG_SWORD, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
    { DAGGER, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
    { SMALL_SHIELD, 3, ARMOR_CLASS, 1, UNDEF_BLESS },
    { FOOD_RATION, 0, FOOD_CLASS, 1, 0 },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Yeoman[] = {
    { SHORT_SWORD, 2, WEAPON_CLASS, 1, UNDEF_BLESS },
    { PARTISAN, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
    { LEATHER_ARMOR, 1, ARMOR_CLASS, 1, UNDEF_BLESS },
    { HIGH_BOOTS, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { LEATHER_GLOVES, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { APPLE, 0, FOOD_CLASS, 10, 0 },
    { CARROT, 0, FOOD_CLASS, 10, 0 },
    { POT_WATER, 0, POTION_CLASS, 3, 0 },
    { 0, 0, 0, 0, 0 }
};
static struct trobj Wizard[] = {
#define W_MULTSTART 2
#define W_MULTEND 6
    { QUARTERSTAFF, 1, WEAPON_CLASS, 1, 1 },
    { CLOAK_OF_MAGIC_RESISTANCE, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
    { UNDEF_TYP, UNDEF_SPE, WAND_CLASS, 1, UNDEF_BLESS },
    { UNDEF_TYP, UNDEF_SPE, RING_CLASS, 2, UNDEF_BLESS },
    { UNDEF_TYP, UNDEF_SPE, POTION_CLASS, 3, UNDEF_BLESS },
    { UNDEF_TYP, UNDEF_SPE, SCROLL_CLASS, 3, UNDEF_BLESS },
    { SPE_FORCE_BOLT, 0, SPBOOK_CLASS, 1, 1 },
    { UNDEF_TYP, UNDEF_SPE, SPBOOK_CLASS, 1, UNDEF_BLESS },
    { 0, 0, 0, 0, 0 }
};

/*
 *      Optional extra inventory items.
 */

static struct trobj Tinopener[] = { { TIN_OPENER, 0, TOOL_CLASS, 1, 0 },
                                    { 0, 0, 0, 0, 0 } };
static struct trobj Magicmarker[] = { { MAGIC_MARKER, UNDEF_SPE, TOOL_CLASS,
                                        1, 0 },
                                      { 0, 0, 0, 0, 0 } };
static struct trobj Lamp[] = { { OIL_LAMP, 1, TOOL_CLASS, 1, 0 },
                               { 0, 0, 0, 0, 0 } };
static struct trobj Blindfold[] = { { BLINDFOLD, 0, TOOL_CLASS, 1, 0 },
                                    { 0, 0, 0, 0, 0 } };
static struct trobj Instrument[] = { { WOODEN_FLUTE, 0, TOOL_CLASS, 1, 0 },
                                     { 0, 0, 0, 0, 0 } };
static struct trobj Xtra_food[] = { { UNDEF_TYP, UNDEF_SPE, FOOD_CLASS, 2, 0 },
                                    { 0, 0, 0, 0, 0 } };
static struct trobj Leash[] = { { LEASH, 0, TOOL_CLASS, 1, 0 },
                                { 0, 0, 0, 0, 0 } };
static struct trobj Towel[] = { { TOWEL, 0, TOOL_CLASS, 1, 0 },
                                { 0, 0, 0, 0, 0 } };
static struct trobj Wishing[] = { { WAN_WISHING, 3, WAND_CLASS, 1, 0 },
                                  { 0, 0, 0, 0, 0 } };
static struct trobj Money[] = { { GOLD_PIECE, 0, COIN_CLASS, 1, 0 },
                                { 0, 0, 0, 0, 0 } };

/* race-based substitutions for initial inventory;
   the weaker cloak for elven rangers is intentional--they shoot better */
static struct inv_sub {
    short race_pm, item_otyp, subs_otyp;
} inv_subs[] = {
    { PM_ELF, DAGGER, ELVEN_DAGGER },
    { PM_ELF, SPEAR, ELVEN_SPEAR },
    { PM_ELF, SHORT_SWORD, ELVEN_SHORT_SWORD },
    { PM_ELF, BOW, ELVEN_BOW },
    { PM_ELF, ARROW, ELVEN_ARROW },
    { PM_ELF, HELMET, ELVEN_LEATHER_HELM },
    /* { PM_ELF, SMALL_SHIELD, ELVEN_SHIELD }, */
    { PM_ELF, CLOAK_OF_DISPLACEMENT, ELVEN_CLOAK },
    { PM_ELF, CRAM_RATION, LEMBAS_WAFER },
    { PM_ORC, DAGGER, ORCISH_DAGGER },
    { PM_ORC, SPEAR, ORCISH_SPEAR },
    { PM_ORC, SHORT_SWORD, ORCISH_SHORT_SWORD },
    { PM_ORC, BOW, ORCISH_BOW },
    { PM_ORC, ARROW, ORCISH_ARROW },
    { PM_ORC, HELMET, ORCISH_HELM },
    { PM_ORC, SMALL_SHIELD, ORCISH_SHIELD },
    { PM_ORC, RING_MAIL, ORCISH_RING_MAIL },
    { PM_ORC, CHAIN_MAIL, ORCISH_CHAIN_MAIL },
    { PM_ORC, CRAM_RATION, TRIPE_RATION },
    { PM_ORC, LEMBAS_WAFER, TRIPE_RATION },
    { PM_DWARF, SPEAR, DWARVISH_SPEAR },
    { PM_DWARF, SHORT_SWORD, DWARVISH_SHORT_SWORD },
    { PM_DWARF, HELMET, DWARVISH_IRON_HELM },
    /* { PM_DWARF, SMALL_SHIELD, DWARVISH_ROUNDSHIELD }, */
    /* { PM_DWARF, PICK_AXE, DWARVISH_MATTOCK }, */
    { PM_DWARF, LEMBAS_WAFER, CRAM_RATION },
    { PM_GNOME, BOW, CROSSBOW },
    { PM_GNOME, ARROW, CROSSBOW_BOLT },
    { PM_DROW, DAGGER, DARK_ELVEN_DAGGER },
    { PM_DROW, SHORT_SWORD, DARK_ELVEN_SHORT_SWORD },
    { PM_DROW, BOW, DARK_ELVEN_BOW },
    { PM_DROW, ARROW, DARK_ELVEN_ARROW },
    { NON_PM, STRANGE_OBJECT, STRANGE_OBJECT }
};

static const struct def_skill Skill_A[] = {
    { P_DAGGER, P_BASIC },
    { P_KNIFE, P_BASIC },
    { P_PICK_AXE, P_EXPERT },
    { P_SHORT_SWORD, P_BASIC },
    { P_SCIMITAR, P_SKILLED },
    { P_SABER, P_EXPERT },
    { P_CLUB, P_SKILLED },
    { P_QUARTERSTAFF, P_SKILLED },
    { P_SLING, P_SKILLED },
    { P_DART, P_BASIC },
    { P_BOOMERANG, P_EXPERT },
    { P_WHIP, P_EXPERT },
    { P_UNICORN_HORN, P_SKILLED },
    { P_ATTACK_SPELL, P_BASIC },
    { P_HEALING_SPELL, P_BASIC },
    { P_DIVINATION_SPELL, P_EXPERT },
    { P_MATTER_SPELL, P_BASIC },
    { P_RIDING, P_BASIC },
    { P_TWO_WEAPON_COMBAT, P_BASIC },
    { P_BARE_HANDED_COMBAT, P_EXPERT },
    { P_NONE, 0 }
};
static const struct def_skill Skill_B[] = {
    { P_DAGGER, P_BASIC },
    { P_AXE, P_EXPERT },
    { P_PICK_AXE, P_SKILLED },
    { P_SHORT_SWORD, P_EXPERT },
    { P_BROAD_SWORD, P_SKILLED },
    { P_LONG_SWORD, P_SKILLED },
    { P_TWO_HANDED_SWORD, P_EXPERT },
    { P_SCIMITAR, P_SKILLED },
    { P_SABER, P_BASIC },
    { P_CLUB, P_SKILLED },
    { P_MACE, P_SKILLED },
    { P_MORNING_STAR, P_SKILLED },
    { P_FLAIL, P_BASIC },
    { P_HAMMER, P_EXPERT },
    { P_QUARTERSTAFF, P_BASIC },
    { P_SPEAR, P_SKILLED },
    { P_TRIDENT, P_SKILLED },
    { P_BOW, P_BASIC },
    { P_ATTACK_SPELL, P_BASIC },
    { P_ESCAPE_SPELL, P_BASIC }, /* special spell is haste self */
    { P_RIDING, P_BASIC },
    { P_TWO_WEAPON_COMBAT, P_BASIC },
    { P_BARE_HANDED_COMBAT, P_MASTER },
    { P_NONE, 0 }
};
static const struct def_skill Skill_Bea[] = {
    { P_DAGGER, P_SKILLED },
    { P_KNIFE, P_EXPERT },
    { P_AXE, P_BASIC },
    { P_SHORT_SWORD, P_BASIC },
    { P_SCIMITAR, P_SKILLED },
    { P_SABER, P_SKILLED },
    { P_CLUB, P_EXPERT },
    { P_TRIDENT, P_EXPERT },
    { P_SLING, P_SKILLED },
    { P_DART, P_BASIC },
    { P_BOOMERANG, P_EXPERT },
    { P_WHIP, P_EXPERT },
    { P_UNICORN_HORN, P_SKILLED },
    { P_ATTACK_SPELL, P_BASIC },
    { P_HEALING_SPELL, P_BASIC },
    { P_MATTER_SPELL, P_EXPERT },
    { P_TWO_WEAPON_COMBAT, P_EXPERT },
    { P_BARE_HANDED_COMBAT, P_EXPERT },
    { P_NONE, 0 }
};
static const struct def_skill Skill_C[] = {
    { P_DAGGER, P_BASIC },
    { P_KNIFE, P_SKILLED },
    { P_AXE, P_SKILLED },
    { P_PICK_AXE, P_BASIC },
    { P_CLUB, P_EXPERT },
    { P_MACE, P_EXPERT },
    { P_MORNING_STAR, P_BASIC },
    { P_FLAIL, P_SKILLED },
    { P_HAMMER, P_SKILLED },
    { P_QUARTERSTAFF, P_EXPERT },
    { P_POLEARMS, P_SKILLED },
    { P_SPEAR, P_EXPERT },
    { P_TRIDENT, P_SKILLED },
    { P_BOW, P_SKILLED },
    { P_SLING, P_EXPERT },
    { P_ATTACK_SPELL, P_BASIC },
    { P_MATTER_SPELL, P_SKILLED },
    { P_BOOMERANG, P_EXPERT },
    { P_UNICORN_HORN, P_BASIC },
    { P_BARE_HANDED_COMBAT, P_MASTER },
    { P_NONE, 0 }
};
static const struct def_skill Skill_D[] = {
    { P_KNIFE, P_BASIC },
    { P_AXE, P_SKILLED },
    { P_PICK_AXE, P_BASIC },
    { P_CLUB, P_BASIC },
    { P_MACE, P_SKILLED },
    { P_MORNING_STAR, P_EXPERT },
	{ P_TWO_HANDED_SWORD, P_SKILLED},
    { P_FLAIL, P_BASIC },
    { P_HAMMER, P_BASIC },
    { P_POLEARMS, P_EXPERT },
    { P_SPEAR, P_SKILLED },
    { P_TRIDENT, P_BASIC },
    { P_LANCE, P_EXPERT },
    { P_BOW, P_BASIC },
    { P_CROSSBOW, P_EXPERT },
    { P_ATTACK_SPELL, P_SKILLED },
    { P_HEALING_SPELL, P_SKILLED },
    { P_CLERIC_SPELL, P_SKILLED },
    { P_RIDING, P_EXPERT },
    { P_BARE_HANDED_COMBAT, P_EXPERT },
    { P_NONE, 0 }
};
static const struct def_skill Skill_F[] = {
    { P_DAGGER, P_EXPERT },
    { P_KNIFE, P_SKILLED },
    { P_AXE, P_BASIC },
    { P_SHORT_SWORD, P_EXPERT },
    { P_LONG_SWORD, P_SKILLED }, 
    { P_SCIMITAR, P_SKILLED },
    { P_SABER, P_SKILLED },
    { P_MACE, P_BASIC },
    { P_HAMMER, P_BASIC },
    { P_QUARTERSTAFF, P_SKILLED },
    { P_WHIP, P_BASIC },
    { P_UNICORN_HORN, P_SKILLED },
    { P_ATTACK_SPELL, P_SKILLED },
    { P_HEALING_SPELL, P_BASIC },
    { P_DIVINATION_SPELL, P_EXPERT },
    { P_ENCHANTMENT_SPELL, P_BASIC },
    { P_MATTER_SPELL, P_EXPERT },
    { P_TWO_WEAPON_COMBAT, P_SKILLED },
    { P_BARE_HANDED_COMBAT, P_SKILLED },
    { P_NONE, 0 }
};
static const struct def_skill Skill_G[] = {
    { P_KNIFE, P_EXPERT },
    { P_FIREARM, P_EXPERT },
    { P_DART, P_SKILLED },
    { P_ESCAPE_SPELL, P_SKILLED },
    { P_RIDING, P_EXPERT },
    { P_BARE_HANDED_COMBAT, P_SKILLED },
    { P_NONE, 0 }
};
static const struct def_skill Skill_H[] = {
    { P_DAGGER, P_SKILLED },
    { P_KNIFE, P_EXPERT },
    { P_SHORT_SWORD, P_SKILLED },
    { P_SCIMITAR, P_BASIC },
    { P_SABER, P_BASIC },
    { P_CLUB, P_SKILLED },
    { P_MACE, P_BASIC },
    { P_QUARTERSTAFF, P_EXPERT },
    { P_POLEARMS, P_BASIC },
    { P_SPEAR, P_BASIC },
    { P_TRIDENT, P_BASIC },
    { P_SLING, P_SKILLED },
    { P_DART, P_EXPERT },
    { P_SHURIKEN, P_SKILLED },
    { P_UNICORN_HORN, P_EXPERT },
    { P_HEALING_SPELL, P_EXPERT },
	{ P_PROTECTION_SPELL, P_SKILLED },
    { P_BARE_HANDED_COMBAT, P_BASIC },
    { P_NONE, 0 }
};
static const struct def_skill Skill_I[] = {
    { P_DAGGER, P_EXPERT },
    { P_KNIFE, P_SKILLED },
    { P_LONG_SWORD, P_SKILLED },
    { P_QUARTERSTAFF, P_SKILLED },
    { P_POLEARMS, P_BASIC },
    { P_SPEAR, P_BASIC },
    { P_TRIDENT, P_BASIC },
    { P_LANCE, P_BASIC },
    { P_WHIP, P_BASIC },
    { P_UNICORN_HORN, P_SKILLED },
    { P_ATTACK_SPELL, P_BASIC },
    { P_HEALING_SPELL, P_SKILLED },
    { P_DIVINATION_SPELL, P_BASIC },
    { P_ENCHANTMENT_SPELL, P_EXPERT },
    { P_MATTER_SPELL, P_EXPERT },
    { P_TWO_WEAPON_COMBAT, P_SKILLED },
    { P_BARE_HANDED_COMBAT, P_SKILLED },
    { P_NONE, 0 }
};
static const struct def_skill Skill_K[] = {
    { P_DAGGER, P_BASIC },
    { P_KNIFE, P_BASIC },
    { P_AXE, P_SKILLED },
    { P_PICK_AXE, P_BASIC },
    { P_SHORT_SWORD, P_SKILLED },
    { P_BROAD_SWORD, P_SKILLED },
    { P_LONG_SWORD, P_EXPERT },
    { P_TWO_HANDED_SWORD, P_SKILLED },
    { P_SCIMITAR, P_BASIC },
    { P_SABER, P_SKILLED },
    { P_CLUB, P_BASIC },
    { P_MACE, P_SKILLED },
    { P_MORNING_STAR, P_SKILLED },
    { P_FLAIL, P_BASIC },
    { P_HAMMER, P_BASIC },
    { P_POLEARMS, P_SKILLED },
    { P_SPEAR, P_SKILLED },
    { P_TRIDENT, P_BASIC },
    { P_LANCE, P_EXPERT },
    { P_BOW, P_BASIC },
    { P_CROSSBOW, P_SKILLED },
    { P_ATTACK_SPELL, P_SKILLED },
    { P_HEALING_SPELL, P_SKILLED },
    { P_CLERIC_SPELL, P_SKILLED },
    { P_RIDING, P_EXPERT },
    { P_TWO_WEAPON_COMBAT, P_SKILLED },
    { P_BARE_HANDED_COMBAT, P_EXPERT },
    { P_NONE, 0 }
};
static const struct def_skill Skill_Mon[] = {
    { P_QUARTERSTAFF, P_BASIC },
    { P_SPEAR, P_BASIC },
    { P_CROSSBOW, P_BASIC },
    { P_SHURIKEN, P_BASIC },
    { P_ATTACK_SPELL, P_BASIC },
    { P_HEALING_SPELL, P_EXPERT },
    { P_DIVINATION_SPELL, P_BASIC },
    { P_ENCHANTMENT_SPELL, P_BASIC },
    { P_CLERIC_SPELL, P_SKILLED },
    { P_ESCAPE_SPELL, P_SKILLED },
    { P_MATTER_SPELL, P_BASIC },
    { P_PROTECTION_SPELL, P_BASIC },
    { P_MARTIAL_ARTS, P_GRAND_MASTER },
    { P_NONE, 0 }
};
static const struct def_skill Skill_N[] = {
    { P_DAGGER, P_BASIC },
    { P_KNIFE, P_BASIC },
    { P_AXE, P_BASIC },
    { P_PICK_AXE, P_BASIC },
    { P_SHORT_SWORD, P_BASIC },
    { P_BROAD_SWORD, P_BASIC },
    { P_LONG_SWORD, P_BASIC },
    { P_TWO_HANDED_SWORD, P_BASIC },
    { P_SCIMITAR, P_BASIC },
    { P_SABER, P_BASIC },
    { P_MACE, P_BASIC },
    { P_MORNING_STAR, P_BASIC },
    { P_FLAIL, P_BASIC },
    { P_HAMMER, P_BASIC },
    { P_QUARTERSTAFF, P_BASIC },
    { P_POLEARMS, P_BASIC },
    { P_SPEAR, P_BASIC },
    { P_TRIDENT, P_BASIC },
    { P_LANCE, P_BASIC },
    { P_BOW, P_BASIC },
    { P_SLING, P_BASIC },
    { P_CROSSBOW, P_BASIC },
    { P_DART, P_BASIC },
    { P_SHURIKEN, P_BASIC },
    { P_BOOMERANG, P_BASIC },
    { P_WHIP, P_BASIC },
    { P_UNICORN_HORN, P_BASIC },
    { P_ATTACK_SPELL, P_BASIC },
    { P_HEALING_SPELL, P_BASIC },
    { P_DIVINATION_SPELL, P_BASIC },
    { P_ENCHANTMENT_SPELL, P_BASIC },
    { P_CLERIC_SPELL, P_BASIC },
    { P_ESCAPE_SPELL, P_BASIC },
    { P_MATTER_SPELL, P_BASIC },
    { P_RIDING, P_BASIC },
    { P_TWO_WEAPON_COMBAT, P_BASIC },
    { P_BARE_HANDED_COMBAT, P_BASIC },
    { P_NONE, 0 }
};
static const struct def_skill Skill_Nec[] = {
    { P_DAGGER, P_EXPERT },
	{ P_KNIFE, P_SKILLED},
    { P_AXE, P_SKILLED },
    { P_CLUB, P_SKILLED },
    { P_MACE, P_BASIC },
    { P_QUARTERSTAFF, P_EXPERT },
    { P_POLEARMS, P_SKILLED },
    { P_SPEAR, P_BASIC },
    { P_TRIDENT, P_BASIC },
	{ P_PICK_AXE, P_SKILLED },
    { P_SLING, P_SKILLED },
    { P_DART, P_EXPERT },
    { P_SHURIKEN, P_BASIC },
    { P_ATTACK_SPELL, P_EXPERT },
    { P_MATTER_SPELL, P_EXPERT },
    { P_BARE_HANDED_COMBAT, P_BASIC },
    { P_NONE, 0 }
};
static const struct def_skill Skill_Pir[] = {
    { P_DAGGER, P_SKILLED },
    { P_KNIFE, P_EXPERT },
    { P_AXE, P_SKILLED },
    { P_SHORT_SWORD, P_BASIC },
    { P_BROAD_SWORD, P_EXPERT },
    { P_LONG_SWORD, P_BASIC },
    { P_SCIMITAR, P_EXPERT },
    { P_SABER, P_EXPERT },
    { P_MORNING_STAR, P_SKILLED },
    { P_FLAIL, P_EXPERT },
    { P_SPEAR, P_SKILLED },
    { P_TRIDENT, P_EXPERT },
    { P_FIREARM, P_EXPERT },
    { P_CROSSBOW, P_EXPERT },
    { P_DART, P_SKILLED },
    { P_WHIP, P_SKILLED },
	{ P_PICK_AXE, P_BASIC},
    { P_UNICORN_HORN, P_BASIC },
    { P_DIVINATION_SPELL, P_BASIC },
    { P_ENCHANTMENT_SPELL, P_BASIC },
    { P_ESCAPE_SPELL, P_SKILLED },
    { P_TWO_WEAPON_COMBAT, P_SKILLED },
    { P_BARE_HANDED_COMBAT, P_EXPERT },
    { P_NONE, 0 }
};
static const struct def_skill Skill_P[] = {
    { P_CLUB, P_EXPERT },
    { P_MACE, P_EXPERT },
    { P_MORNING_STAR, P_EXPERT },
    { P_FLAIL, P_EXPERT },
    { P_HAMMER, P_EXPERT },
    { P_QUARTERSTAFF, P_EXPERT },
    { P_POLEARMS, P_SKILLED },
    { P_SPEAR, P_SKILLED },
    { P_TRIDENT, P_SKILLED },
    { P_LANCE, P_BASIC },
    { P_BOW, P_BASIC },
    { P_SLING, P_BASIC },
    { P_CROSSBOW, P_BASIC },
    { P_DART, P_BASIC },
    { P_SHURIKEN, P_BASIC },
    { P_BOOMERANG, P_BASIC },
    { P_UNICORN_HORN, P_SKILLED },
    { P_HEALING_SPELL, P_EXPERT },
    { P_DIVINATION_SPELL, P_EXPERT },
    { P_CLERIC_SPELL, P_EXPERT },
    { P_BARE_HANDED_COMBAT, P_BASIC },
    { P_NONE, 0 }
};
static const struct def_skill Skill_R[] = {
    { P_DAGGER, P_EXPERT },
    { P_KNIFE, P_EXPERT },
    { P_SHORT_SWORD, P_EXPERT },
    { P_BROAD_SWORD, P_SKILLED },
    { P_LONG_SWORD, P_SKILLED },
    { P_TWO_HANDED_SWORD, P_BASIC },
    { P_SCIMITAR, P_SKILLED },
    { P_SABER, P_SKILLED },
    { P_CLUB, P_SKILLED },
    { P_MACE, P_SKILLED },
    { P_MORNING_STAR, P_BASIC },
    { P_FLAIL, P_BASIC },
    { P_HAMMER, P_BASIC },
    { P_POLEARMS, P_BASIC },
    { P_SPEAR, P_BASIC },
    { P_CROSSBOW, P_EXPERT },
    { P_DART, P_EXPERT },
    { P_SHURIKEN, P_SKILLED },
    { P_DIVINATION_SPELL, P_SKILLED },
    { P_ESCAPE_SPELL, P_SKILLED },
    { P_MATTER_SPELL, P_SKILLED },
    { P_RIDING, P_BASIC },
    { P_TWO_WEAPON_COMBAT, P_EXPERT },
    { P_BARE_HANDED_COMBAT, P_EXPERT },
    { P_NONE, 0 }
};
static const struct def_skill Skill_Ran[] = {
    { P_DAGGER, P_EXPERT },
    { P_KNIFE, P_SKILLED },
    { P_AXE, P_SKILLED },
    { P_PICK_AXE, P_BASIC },
    { P_SHORT_SWORD, P_BASIC },
    { P_MORNING_STAR, P_BASIC },
    { P_FLAIL, P_SKILLED },
    { P_HAMMER, P_BASIC },
    { P_QUARTERSTAFF, P_BASIC },
    { P_POLEARMS, P_SKILLED },
    { P_SPEAR, P_EXPERT },
    { P_TRIDENT, P_BASIC },
    { P_BOW, P_EXPERT },
    { P_SLING, P_EXPERT },
    { P_CROSSBOW, P_EXPERT },
    { P_DART, P_EXPERT },
    { P_SHURIKEN, P_SKILLED },
    { P_BOOMERANG, P_EXPERT },
    { P_WHIP, P_BASIC },
    { P_HEALING_SPELL, P_BASIC },
    { P_DIVINATION_SPELL, P_EXPERT },
    { P_ESCAPE_SPELL, P_BASIC },
    { P_RIDING, P_BASIC },
    { P_BARE_HANDED_COMBAT, P_BASIC },
    { P_NONE, 0 }
};
static const struct def_skill Skill_S[] = {
    { P_DAGGER, P_BASIC },
    { P_KNIFE, P_SKILLED },
    { P_SHORT_SWORD, P_EXPERT },
    { P_BROAD_SWORD, P_SKILLED },
    { P_LONG_SWORD, P_EXPERT },
    { P_TWO_HANDED_SWORD, P_EXPERT },
    { P_SCIMITAR, P_BASIC },
    { P_SABER, P_BASIC },
    { P_FLAIL, P_SKILLED },
    { P_QUARTERSTAFF, P_BASIC },
    { P_POLEARMS, P_SKILLED },
    { P_SPEAR, P_SKILLED },
    { P_LANCE, P_SKILLED },
    { P_BOW, P_EXPERT },
    { P_SHURIKEN, P_EXPERT },
    { P_ATTACK_SPELL, P_BASIC },
    { P_DIVINATION_SPELL, P_BASIC }, /* special spell is clairvoyance */
    { P_CLERIC_SPELL, P_SKILLED },
    { P_RIDING, P_SKILLED },
    { P_TWO_WEAPON_COMBAT, P_EXPERT },
    { P_MARTIAL_ARTS, P_MASTER },
    { P_NONE, 0 }
};
static const struct def_skill Skill_Shi[] = {
    { P_DAGGER, P_BASIC },
    { P_KNIFE, P_EXPERT },
    { P_SHORT_SWORD, P_EXPERT },
    { P_BROAD_SWORD, P_BASIC },
    { P_LONG_SWORD, P_BASIC },
    { P_TWO_HANDED_SWORD, P_BASIC },
    { P_SCIMITAR, P_BASIC },
    { P_SABER, P_BASIC },
    { P_FLAIL, P_EXPERT },
    { P_QUARTERSTAFF, P_SKILLED },
    { P_POLEARMS, P_SKILLED },
    { P_SPEAR, P_SKILLED },
    { P_LANCE, P_SKILLED },
    { P_DART, P_EXPERT },
    { P_SHURIKEN, P_EXPERT },
    { P_ATTACK_SPELL, P_BASIC },
    { P_DIVINATION_SPELL, P_BASIC },
	{ P_ESCAPE_SPELL, P_EXPERT },
    { P_TWO_WEAPON_COMBAT, P_EXPERT },
    { P_MARTIAL_ARTS, P_MASTER },
    { P_NONE, 0 }
};
static const struct def_skill Skill_T[] = {
    { P_DAGGER, P_EXPERT },
    { P_KNIFE, P_SKILLED },
    { P_AXE, P_BASIC },
    { P_PICK_AXE, P_BASIC },
    { P_SHORT_SWORD, P_EXPERT },
    { P_BROAD_SWORD, P_BASIC },
    { P_LONG_SWORD, P_BASIC },
    { P_TWO_HANDED_SWORD, P_BASIC },
    { P_SCIMITAR, P_SKILLED },
    { P_SABER, P_SKILLED },
    { P_MACE, P_BASIC },
    { P_MORNING_STAR, P_BASIC },
    { P_FLAIL, P_BASIC },
    { P_HAMMER, P_BASIC },
    { P_QUARTERSTAFF, P_BASIC },
    { P_POLEARMS, P_BASIC },
    { P_SPEAR, P_BASIC },
    { P_TRIDENT, P_BASIC },
    { P_LANCE, P_BASIC },
    { P_BOW, P_BASIC },
    { P_SLING, P_BASIC },
    { P_CROSSBOW, P_BASIC },
    { P_DART, P_EXPERT },
    { P_SHURIKEN, P_BASIC },
    { P_BOOMERANG, P_BASIC },
    { P_WHIP, P_BASIC },
    { P_UNICORN_HORN, P_SKILLED },
    { P_DIVINATION_SPELL, P_BASIC },
    { P_ENCHANTMENT_SPELL, P_BASIC },
    { P_ESCAPE_SPELL, P_SKILLED },
    { P_RIDING, P_BASIC },
    { P_TWO_WEAPON_COMBAT, P_SKILLED },
    { P_BARE_HANDED_COMBAT, P_SKILLED },
    { P_NONE, 0 }
};
static const struct def_skill Skill_U[] = {
    { P_DAGGER, P_EXPERT },
    { P_LONG_SWORD, P_SKILLED },
    { P_CLUB, P_SKILLED },
    { P_MACE, P_SKILLED },
    { P_MORNING_STAR, P_SKILLED },
    { P_FLAIL, P_SKILLED },
    { P_HAMMER, P_SKILLED },
    { P_QUARTERSTAFF, P_SKILLED },
    { P_POLEARMS, P_SKILLED },
    { P_SPEAR, P_EXPERT },
    { P_BOW, P_BASIC },
    { P_SLING, P_BASIC },
    { P_FIREARM, P_EXPERT },
    { P_CROSSBOW, P_EXPERT },
    { P_DART, P_BASIC },
    { P_SHURIKEN, P_BASIC },
    { P_BOOMERANG, P_BASIC },
    { P_WHIP, P_EXPERT },
    { P_UNICORN_HORN, P_SKILLED },
    { P_MATTER_SPELL, P_BASIC },
    { P_BARE_HANDED_COMBAT, P_GRAND_MASTER },
    { P_NONE, 0 }
};
static const struct def_skill Skill_V[] = {
    { P_DAGGER, P_EXPERT },
    { P_AXE, P_EXPERT },
    { P_PICK_AXE, P_SKILLED },
    { P_SHORT_SWORD, P_SKILLED },
    { P_BROAD_SWORD, P_SKILLED },
    { P_LONG_SWORD, P_EXPERT },
    { P_TWO_HANDED_SWORD, P_EXPERT },
    { P_SCIMITAR, P_BASIC },
    { P_SABER, P_BASIC },
    { P_HAMMER, P_EXPERT },
    { P_QUARTERSTAFF, P_BASIC },
    { P_POLEARMS, P_SKILLED },
    { P_SPEAR, P_SKILLED },
    { P_TRIDENT, P_BASIC },
    { P_LANCE, P_SKILLED },
    { P_SLING, P_BASIC },
    { P_ATTACK_SPELL, P_BASIC },
    { P_ESCAPE_SPELL, P_BASIC },
    { P_RIDING, P_SKILLED },
    { P_TWO_WEAPON_COMBAT, P_SKILLED },
    { P_BARE_HANDED_COMBAT, P_EXPERT },
    { P_NONE, 0 }
};
static const struct def_skill Skill_Y[] = {
    { P_DAGGER, P_SKILLED },
    { P_KNIFE, P_BASIC },
    { P_AXE, P_SKILLED },
    { P_SHORT_SWORD, P_EXPERT },
    { P_BROAD_SWORD, P_BASIC },
    { P_LONG_SWORD, P_SKILLED },
    { P_SABER, P_SKILLED },
    { P_HAMMER, P_BASIC },
    { P_QUARTERSTAFF, P_SKILLED },
    { P_POLEARMS, P_EXPERT },
    { P_SPEAR, P_BASIC },
    { P_TRIDENT, P_SKILLED },
    { P_LANCE, P_SKILLED },
    { P_BOW, P_EXPERT },
    { P_SLING, P_BASIC },
    { P_FIREARM, P_SKILLED },
    { P_CROSSBOW, P_SKILLED },
    { P_DART, P_BASIC },
    { P_WHIP, P_BASIC },
    { P_UNICORN_HORN, P_BASIC },
    { P_ENCHANTMENT_SPELL, P_SKILLED },
    { P_RIDING, P_EXPERT },
    { P_TWO_WEAPON_COMBAT, P_SKILLED },
    { P_BARE_HANDED_COMBAT, P_SKILLED },
    { P_NONE, 0 }
};
static const struct def_skill Skill_W[] = {
    { P_DAGGER, P_EXPERT },
    { P_KNIFE, P_SKILLED },
    { P_AXE, P_SKILLED },
    { P_SHORT_SWORD, P_BASIC },
    { P_CLUB, P_SKILLED },
    { P_MACE, P_BASIC },
    { P_QUARTERSTAFF, P_EXPERT },
    { P_POLEARMS, P_SKILLED },
    { P_SPEAR, P_BASIC },
    { P_TRIDENT, P_BASIC },
    { P_SLING, P_SKILLED },
    { P_DART, P_EXPERT },
    { P_SHURIKEN, P_BASIC },
    { P_ATTACK_SPELL, P_EXPERT },
    { P_HEALING_SPELL, P_SKILLED },
    { P_DIVINATION_SPELL, P_EXPERT },
    { P_ENCHANTMENT_SPELL, P_SKILLED },
    { P_CLERIC_SPELL, P_SKILLED },
    { P_ESCAPE_SPELL, P_EXPERT },
    { P_MATTER_SPELL, P_EXPERT },
    { P_PROTECTION_SPELL, P_SKILLED },
    { P_RIDING, P_BASIC },
    { P_BARE_HANDED_COMBAT, P_BASIC },
    { P_NONE, 0 }
};

STATIC_OVL void
knows_object(obj)
register int obj;
{
    discover_object(obj, TRUE, FALSE);
    objects[obj].oc_pre_discovered = 1; /* not a "discovery" */
}

/* Know ordinary (non-magical) objects of a certain class,
 * like all gems except the loadstone and luckstone.
 */
STATIC_OVL void
knows_class(sym)
register char sym;
{
    register int ct;
    for (ct = 1; ct < NUM_OBJECTS; ct++)
        if (objects[ct].oc_class == sym && !objects[ct].oc_magic)
            knows_object(ct);
}

void
u_init()
{
    register int i;
    struct u_roleplay tmpuroleplay = u.uroleplay; /* set by rcfile options */

    flags.female = flags.initgend;
    flags.beginner = 1;

    /* zero u, including pointer values --
     * necessary when aborting from a failed restore */
    (void) memset((genericptr_t) &u, 0, sizeof(u));
    u.ustuck = (struct monst *) 0;
    (void) memset((genericptr_t) &ubirthday, 0, sizeof(ubirthday));
    (void) memset((genericptr_t) &urealtime, 0, sizeof(urealtime));

    u.uroleplay = tmpuroleplay; /* restore options set via rcfile */

#if 0  /* documentation of more zero values as desirable */
    u.usick_cause[0] = 0;
    u.uluck  = u.moreluck = 0;
    uarmu = 0;
    uarm = uarmc = uarmh = uarms = uarmg = uarmf = 0;
    uwep = uball = uchain = uleft = uright = 0;
    uswapwep = uquiver = 0;
    u.twoweap = 0;
    u.ublessed = 0;                     /* not worthy yet */
    u.ugangr   = 0;                     /* gods not angry */
    u.ugifts   = 0;                     /* no divine gifts bestowed */
    u.uevent.uhand_of_elbereth = 0;
    u.uevent.uheard_tune = 0;
    u.uevent.uopened_dbridge = 0;
    u.uevent.udemigod = 0;              /* not a demi-god yet... */
    u.udg_cnt = 0;
    u.mh = u.mhmax = u.mtimedone = 0;
    u.uz.dnum = u.uz0.dnum = 0;
    u.utotype = 0;
#endif /* 0 */

    u.uz.dlevel = 1;
    u.uz0.dlevel = 0;
    u.utolev = u.uz;

    u.umoved = FALSE;
    u.umortality = 0;
    u.ugrave_arise = NON_PM;

    u.umonnum = u.umonster = (flags.female && urole.femalenum != NON_PM)
                                 ? urole.femalenum
                                 : urole.malenum;
    u.ulycn = NON_PM;
    set_uasmon();

    u.ulevel = 0; /* set up some of the initial attributes */
    u.uhp = u.uhpmax = newhp();
    u.uen = u.uenmax = newpw();
    u.uspellprot = 0;
    adjabil(0, 1);
    u.ulevel = u.ulevelmax = 1;

    init_uhunger();
    for (i = 0; i <= MAXSPELL; i++)
        spl_book[i].sp_id = NO_SPELL;
    u.ublesscnt = 300; /* no prayers just yet */
    u.ualignbase[A_CURRENT] = u.ualignbase[A_ORIGINAL] = u.ualign.type =
        aligns[flags.initalign].value;

#if defined(BSD) && !defined(POSIX_TYPES)
    (void) time((long *) &ubirthday);
#else
    (void) time(&ubirthday);
#endif

    /*
     *  For now, everyone starts out with a night vision range of 1 and
     *  their xray range disabled.
     */
    u.nv_range = 1;
    u.xray_range = -1;

    /*** Role-specific initializations ***/
    switch (Role_switch) {
    /* rn2(100) > 50 necessary for some choices because some
     * random number generators are bad enough to seriously
     * skew the results if we use rn2(2)...  --KAA
     */
    case PM_ARCHEOLOGIST:
        ini_inv(Archeologist);
        if (!rn2(10))
            ini_inv(Tinopener);
        else if (!rn2(4))
            ini_inv(Lamp);
        else if (!rn2(10))
            ini_inv(Magicmarker);
        knows_object(SACK);
        knows_object(TOUCHSTONE);
        skill_init(Skill_A);
        break;
    case PM_BARBARIAN:
        if (rn2(100) >= 50) { /* see above comment */
            Barbarian[B_MAJOR].trotyp = BATTLE_AXE;
            Barbarian[B_MINOR].trotyp = SHORT_SWORD;
        }
        ini_inv(Barbarian);
        if (!rn2(6))
            ini_inv(Lamp);
        knows_class(WEAPON_CLASS);
        knows_class(ARMOR_CLASS);
        skill_init(Skill_B);
        break;
    case PM_BEAST_TAMER:
        ini_inv(Beast_tamer);
        skill_init(Skill_Bea);
        break;
    case PM_CAVEMAN:
        Cave_man[C_AMMO].trquan = rn1(11, 10); /* 10..20 */
        ini_inv(Cave_man);
        skill_init(Skill_C);
        break;
    case PM_DRAGON_RIDER:
        switch (rnd(13)) {
        case 1:
            Dragon_rider[D_POLEARM].trotyp = PARTISAN;
            break;
        case 2:
            Dragon_rider[D_POLEARM].trotyp = FAUCHARD;
            break;
        case 3:
            Dragon_rider[D_POLEARM].trotyp = GLAIVE;
            break;
        case 4:
            Dragon_rider[D_POLEARM].trotyp = BEC_DE_CORBIN;
            break;
        case 5:
            Dragon_rider[D_POLEARM].trotyp = SPETUM;
            break;
        case 6:
            Dragon_rider[D_POLEARM].trotyp = LUCERN_HAMMER;
            break;
        case 7:
            Dragon_rider[D_POLEARM].trotyp = GUISARME;
            break;
        case 8:
            Dragon_rider[D_POLEARM].trotyp = RANSEUR;
            break;
        case 9:
            Dragon_rider[D_POLEARM].trotyp = VOULGE;
            break;
        case 10:
            Dragon_rider[D_POLEARM].trotyp = BILL_GUISARME;
            break;
        case 11:
            Dragon_rider[D_POLEARM].trotyp = BARDICHE;
            break;
        case 12:
            Dragon_rider[D_POLEARM].trotyp = HALBERD;
            break;
        case 13:
            Dragon_rider[D_POLEARM].trotyp = LANCE;
            break;
        default:
            break;
        }
        ini_inv(Dragon_rider);
        if (!rn2(5))
            ini_inv(Lamp);
        knows_class(WEAPON_CLASS);
        knows_class(ARMOR_CLASS);
        skill_init(Skill_D);
        break;
    case PM_FLAME_MAGE:
        switch (rnd(2)) {
        case 1:
            Flame_Mage[F_BOOK].trotyp = SPE_DETECT_MONSTERS;
            break;
        case 2:
            Flame_Mage[F_BOOK].trotyp = SPE_LIGHT;
            break;
        default:
            break;
        }
        ini_inv(Flame_Mage);
        if (!rn2(5))
            ini_inv(Lamp);
        else if (!rn2(5))
            ini_inv(Blindfold);
        else if (!rn2(5))
            ini_inv(Magicmarker);
        skill_init(Skill_F);
        break;
    case PM_GUNSLINGER:
        Gunslinger[G_BULLETS].trquan = rn1(10, 20);
        ini_inv(Gunslinger);
        skill_init(Skill_G);
        break;
    case PM_HEALER:
        u.umoney0 = rn1(1000, 1001);
        ini_inv(Healer);
        if (!rn2(25))
            ini_inv(Lamp);
        knows_object(POT_FULL_HEALING);
        skill_init(Skill_H);
        break;
    case PM_ICE_MAGE:
        switch (rnd(2)) {
        case 1:
            Ice_Mage[I_BOOK].trotyp = SPE_CONFUSE_MONSTER;
            break;
        case 2:
            Ice_Mage[I_BOOK].trotyp = SPE_SLOW_MONSTER;
            break;
        default:
            break;
        }
        ini_inv(Ice_Mage);
        if (!rn2(5))
            ini_inv(Lamp);
        else if (!rn2(5))
            ini_inv(Blindfold);
        else if (!rn2(5))
            ini_inv(Magicmarker);
        skill_init(Skill_I);
        break;
    case PM_KNIGHT:
        ini_inv(Knight);
        knows_class(WEAPON_CLASS);
        knows_class(ARMOR_CLASS);
        /* give knights chess-like mobility--idea from wooledge@..cwru.edu */
        HJumping |= FROMOUTSIDE;
        skill_init(Skill_K);
        break;
    case PM_MONK: {
        static short M_spell[] = { SPE_HEALING, SPE_PROTECTION, SPE_SLEEP };

        Monk[M_BOOK].trotyp = M_spell[rn2(90) / 30]; /* [0..2] */
        ini_inv(Monk);
        if (!rn2(5))
            ini_inv(Magicmarker);
        else if (!rn2(10))
            ini_inv(Lamp);
        knows_class(ARMOR_CLASS);
        /* sufficiently martial-arts oriented item to ignore language issue */
        knows_object(SHURIKEN);
        skill_init(Skill_Mon);
        break;
    }
    case PM_NAKED:
        ini_inv(Naked);
        skill_init(Skill_N);
        break;
    case PM_NECROMANCER:
        switch (rnd(5)) {
        case 1:
            Necromancer[N_BOOK].trotyp = SPE_FORCE_BOLT;
            break;
        case 2:
            Necromancer[N_BOOK].trotyp = SPE_KNOCK;
            break;
        case 3:
            Necromancer[N_BOOK].trotyp = SPE_MAGIC_MISSILE;
            break;
        case 4:
            Necromancer[N_BOOK].trotyp = SPE_CREATE_MONSTER;
            break;
        case 5:
            Necromancer[N_BOOK].trotyp = SPE_WIZARD_LOCK;
            break;
        default:
            break;
        }
        ini_inv(Necromancer);
        knows_class(SPBOOK_CLASS);
        if (!rn2(5))
            ini_inv(Magicmarker);
        if (!rn2(5))
            ini_inv(Blindfold);
        skill_init(Skill_Nec);
        break;
    case PM_PIRATE:
        Pirate[P_BULLETS].trquan = rn1(10, 20);
        ini_inv(Pirate);
        if (!rn2(10))
            ini_inv(Lamp);
        skill_init(Skill_Pir);
        break;
    case PM_PRIEST:
        ini_inv(Priest);
        if (!rn2(10))
            ini_inv(Magicmarker);
        else if (!rn2(10))
            ini_inv(Lamp);
        knows_object(POT_WATER);
        skill_init(Skill_P);
        /* KMH, conduct --
         * Some may claim that this isn't agnostic, since they
         * are literally "priests" and they have holy water.
         * But we don't count it as such.  Purists can always
         * avoid playing priests and/or confirm another player's
         * role in their YAAP.
         */
        break;
    case PM_RANGER:
        Ranger[RAN_TWO_ARROWS].trquan = rn1(10, 50);
        Ranger[RAN_ZERO_ARROWS].trquan = rn1(10, 30);
        ini_inv(Ranger);
        skill_init(Skill_Ran);
        break;
    case PM_ROGUE:
        Rogue[R_DAGGERS].trquan = rn1(10, 6);
        u.umoney0 = 0;
        ini_inv(Rogue);
        if (!rn2(5))
            ini_inv(Blindfold);
        knows_object(SACK);
        skill_init(Skill_R);
        break;
    case PM_SAMURAI:
        Samurai[S_ARROWS].trquan = rn1(20, 26);
        ini_inv(Samurai);
        if (!rn2(5))
            ini_inv(Blindfold);
        knows_class(WEAPON_CLASS);
        knows_class(ARMOR_CLASS);
        skill_init(Skill_S);
        break;
    case PM_SHINOBI:
        Shinobi[Shi_SHURIKEN].trquan = rn1(20, 20);
        ini_inv(Shinobi);
        knows_class(WEAPON_CLASS);
        knows_class(ARMOR_CLASS);
        skill_init(Skill_Shi);
        break;
    case PM_TOURIST:
        Tourist[T_DARTS].trquan = rn1(20, 21);
        u.umoney0 = rnd(1000);
        ini_inv(Tourist);
        if (!rn2(25))
            ini_inv(Tinopener);
        else if (!rn2(25))
            ini_inv(Leash);
        else if (!rn2(25))
            ini_inv(Towel);
        else if (!rn2(25))
            ini_inv(Magicmarker);
        skill_init(Skill_T);
        break;
 	case PM_UNDEAD_SLAYER:
		switch (rn2(100) / 25) {
		    case 0:	// Pistol and silver bullets 
#ifdef FIREARMS
        UndeadSlayer[U_MINOR].trotyp = PISTOL;
        UndeadSlayer[U_RANGE].trotyp = SILVER_BULLET;
        UndeadSlayer[U_RANGE].trquan = rn1(10, 30);
        break;
#endif
    case 1: // Crossbow and bolts 
        UndeadSlayer[U_MINOR].trotyp = CROSSBOW;
        UndeadSlayer[U_RANGE].trotyp = CROSSBOW_BOLT;
        UndeadSlayer[U_RANGE].trquan = rn1(10, 30);
        UndeadSlayer[U_MISC].trotyp = LOW_BOOTS;
        UndeadSlayer[U_MISC].trspe = 1;
        UndeadSlayer[U_ARMOR].trotyp = LEATHER_JACKET;
        UndeadSlayer[U_ARMOR].trspe = 1;
        // helmet & armour are no longer candidates for
        // substitution for orcish versions so no extra
        // food should be given in compensation.
         
      //  if (Race_if(PM_ORC))
        //    no_extra_food = TRUE;
        break;
    case 2: // Whip and daggers
        UndeadSlayer[U_MINOR].trotyp = BULLWHIP;
        UndeadSlayer[U_MINOR].trspe = 2;
        break;
    case 3: // Silver spear and daggers
        break;
    }
    ini_inv(UndeadSlayer);
    knows_class(WEAPON_CLASS);
    knows_class(ARMOR_CLASS);
    if (!rn2(6))
        ini_inv(Lamp);
    skill_init(Skill_U);
    break;
    case PM_VALKYRIE:
        ini_inv(Valkyrie);
        if (!rn2(6))
            ini_inv(Lamp);
        knows_class(WEAPON_CLASS);
        knows_class(ARMOR_CLASS);
        skill_init(Skill_V);
        break;
    case PM_YEOMAN:
        ini_inv(Yeoman);
        knows_class(WEAPON_CLASS);
        knows_class(ARMOR_CLASS);
        skill_init(Skill_Y);
        break;
    case PM_WIZARD:
        ini_inv(Wizard);
        if (!rn2(5))
            ini_inv(Magicmarker);
        if (!rn2(5))
            ini_inv(Blindfold);
        skill_init(Skill_W);
        break;

    default: /* impossible */
        break;
    }

    /*** Race-specific initializations ***/
    switch (Race_switch) {
    case PM_HUMAN:
        /* Nothing special */
        break;

    case PM_ELF:
        /*
         * Elves are people of music and song, or they are warriors.
         * Non-warriors get an instrument.  We use a kludge to
         * get only non-magic instruments.
         */
        if (Role_if(PM_PRIEST) || Role_if(PM_WIZARD)) {
            static int trotyp[] = { WOODEN_FLUTE, TOOLED_HORN, WOODEN_HARP,
                                    BELL,         BUGLE,       LEATHER_DRUM };
            Instrument[0].trotyp = trotyp[rn2(SIZE(trotyp))];
            ini_inv(Instrument);
        }

        /* Elves can recognize all elvish objects */
        knows_object(ELVEN_SHORT_SWORD);
        knows_object(ELVEN_ARROW);
        knows_object(ELVEN_BOW);
        knows_object(ELVEN_SPEAR);
        knows_object(ELVEN_DAGGER);
        knows_object(ELVEN_BROADSWORD);
        knows_object(ELVEN_MITHRIL_COAT);
        knows_object(ELVEN_LEATHER_HELM);
        knows_object(ELVEN_SHIELD);
        knows_object(ELVEN_BOOTS);
        knows_object(ELVEN_CLOAK);
        break;

    case PM_DROW:
        /* Drows can recognize all droven objects */
        knows_object(DARK_ELVEN_SHORT_SWORD);
        knows_object(DARK_ELVEN_ARROW);
        knows_object(DARK_ELVEN_BOW);
        knows_object(DARK_ELVEN_DAGGER);
        knows_object(DARK_ELVEN_MITHRIL_COAT);
        break;

    case PM_DWARF:
        /* Dwarves can recognize all dwarvish objects */
        knows_object(DWARVISH_SPEAR);
        knows_object(DWARVISH_SHORT_SWORD);
        knows_object(DWARVISH_MATTOCK);
        knows_object(DWARVISH_IRON_HELM);
        knows_object(DWARVISH_MITHRIL_COAT);
        knows_object(DWARVISH_CLOAK);
        knows_object(DWARVISH_ROUNDSHIELD);
        break;

    case PM_GNOME:
        break;

    case PM_ORC:
        /* compensate for generally inferior equipment */
        if (!Role_if(PM_WIZARD))
            ini_inv(Xtra_food);
        /* Orcs can recognize all orcish objects */
        knows_object(ORCISH_SHORT_SWORD);
        knows_object(ORCISH_ARROW);
        knows_object(ORCISH_BOW);
        knows_object(ORCISH_SPEAR);
        knows_object(ORCISH_DAGGER);
        knows_object(ORCISH_CHAIN_MAIL);
        knows_object(ORCISH_RING_MAIL);
        knows_object(ORCISH_HELM);
        knows_object(ORCISH_SHIELD);
        knows_object(URUK_HAI_SHIELD);
        knows_object(ORCISH_CLOAK);
        break;

    default: /* impossible */
        break;
    }

    if (discover)
        ini_inv(Wishing);

    if (wizard)
        read_wizkit();

    if (u.umoney0)
        ini_inv(Money);
    u.umoney0 += hidden_gold(); /* in case sack has gold in it */

    find_ac();     /* get initial ac value */
    init_attr(75); /* init attribute values */
    max_rank_sz(); /* set max str size for class ranks */
    /*
     *  Do we really need this?
     */
    for (i = 0; i < A_MAX; i++)
        if (!rn2(20)) {
            register int xd = rn2(7) - 2; /* biased variation */

            (void) adjattrib(i, xd, TRUE);
            if (ABASE(i) < AMAX(i))
                AMAX(i) = ABASE(i);
        }

    /* make sure you can carry all you have - especially for Tourists */
    while (inv_weight() > 0) {
        if (adjattrib(A_STR, 1, TRUE))
            continue;
        if (adjattrib(A_CON, 1, TRUE))
            continue;
        /* only get here when didn't boost strength or constitution */
        break;
    }

    return;
}

/* skills aren't initialized, so we use the role-specific skill lists */
STATIC_OVL boolean
restricted_spell_discipline(otyp)
int otyp;
{
    const struct def_skill *skills;
    int this_skill = spell_skilltype(otyp);

    switch (Role_switch) {
    case PM_ARCHEOLOGIST:
        skills = Skill_A;
        break;
    case PM_BARBARIAN:
        skills = Skill_B;
        break;
    case PM_BEAST_TAMER:
        skills = Skill_Bea;
        break;
    case PM_CAVEMAN:
        skills = Skill_C;
        break;
    case PM_DRAGON_RIDER:
        skills = Skill_D;
        break;
    case PM_FLAME_MAGE:
        skills = Skill_F;
        break;
    case PM_GUNSLINGER:
        skills = Skill_G;
        break;
    case PM_HEALER:
        skills = Skill_H;
        break;
    case PM_ICE_MAGE:
        skills = Skill_I;
        break;
    case PM_KNIGHT:
        skills = Skill_K;
        break;
    case PM_MONK:
        skills = Skill_Mon;
        break;
    case PM_NAKED:
        skills = Skill_N;
        break;
    case PM_NECROMANCER:
        skills = Skill_Nec;
        break;
    case PM_PIRATE:
        skills = Skill_Pir;
        break;
    case PM_PRIEST:
        skills = Skill_P;
        break;
    case PM_RANGER:
        skills = Skill_Ran;
        break;
    case PM_ROGUE:
        skills = Skill_R;
        break;
    case PM_SAMURAI:
        skills = Skill_S;
        break;
    case PM_SHINOBI:
        skills = Skill_Shi;
        break;
    case PM_TOURIST:
        skills = Skill_T;
        break;
    case PM_UNDEAD_SLAYER:
        skills = Skill_U;
        break;
    case PM_VALKYRIE:
        skills = Skill_V;
        break;
    case PM_YEOMAN:
        skills = Skill_Y;
        break;
    case PM_WIZARD:
        skills = Skill_W;
        break;
    default:
        skills = 0; /* lint suppression */
        break;
    }

    while (skills && skills->skill != P_NONE) {
        if (skills->skill == this_skill)
            return FALSE;
        ++skills;
    }
    return TRUE;
}

STATIC_OVL void
ini_inv(trop)
register struct trobj *trop;
{
    struct obj *obj;
    int otyp, i;

    while (trop->trclass) {
        otyp = (int) trop->trotyp;
        if (otyp != UNDEF_TYP) {
            obj = mksobj(otyp, TRUE, FALSE);
        } else { /* UNDEF_TYP */
            static NEARDATA short nocreate = STRANGE_OBJECT;
            static NEARDATA short nocreate2 = STRANGE_OBJECT;
            static NEARDATA short nocreate3 = STRANGE_OBJECT;
            static NEARDATA short nocreate4 = STRANGE_OBJECT;
            /*
             * For random objects, do not create certain overly powerful
             * items: wand of wishing, ring of levitation, or the
             * polymorph/polymorph control combination.  Specific objects,
             * i.e. the discovery wishing, are still OK.
             * Also, don't get a couple of really useless items.  (Note:
             * punishment isn't "useless".  Some players who start out with
             * one will immediately read it and use the iron ball as a
             * weapon.)
             */
            obj = mkobj(trop->trclass, FALSE);
            otyp = obj->otyp;
            while (otyp == WAN_WISHING || otyp == nocreate
                   || otyp == nocreate2 || otyp == nocreate3
                   || otyp == nocreate4 || otyp == RIN_LEVITATION
                  
				   || ((Role_if(PM_FLAME_MAGE) || Role_if(PM_ICE_MAGE))
                       && (otyp == RIN_FIRE_RESISTANCE
                           || otyp == RIN_COLD_RESISTANCE
                           || otyp == SPE_ENDURE_HEAT
                           || otyp == SPE_ENDURE_COLD))
                   /* KMH -- Hobbits shouldn't get ring of invis. */
                   || (Role_if(PM_HOBBIT) && otyp == RIN_INVISIBILITY)
                   /* 'useless' items */
                   || otyp == POT_HALLUCINATION
                   || otyp == POT_ACID
                   || otyp == SCR_AMNESIA
                   || otyp == SCR_FIRE
                   || otyp == SCR_BLANK_PAPER
                   || otyp == SPE_BLANK_PAPER
                   || otyp == RIN_AGGRAVATE_MONSTER
                   || otyp == RIN_HUNGER
                   || otyp == WAN_NOTHING
                   /* orcs start with poison resistance */
                   || (otyp == RIN_POISON_RESISTANCE && Race_if(PM_ORC))
                   /* Monks don't use weapons */
                   || (otyp == SCR_ENCHANT_WEAPON && Role_if(PM_MONK))
                   /* wizard patch -- they already have one */
                   || (otyp == SPE_FORCE_BOLT && Role_if(PM_WIZARD))
                   /* powerful spells are either useless to
                      low level players or unbalancing; also
                      spells in restricted skill categories */
                   || (obj->oclass == SPBOOK_CLASS
                       && (objects[otyp].oc_level > 3
                           || restricted_spell_discipline(otyp)))) {
                dealloc_obj(obj);
                obj = mkobj(trop->trclass, FALSE);
                otyp = obj->otyp;
            }

            /* Don't start with +0 or negative rings */
            if (objects[otyp].oc_charged && obj->spe <= 0)
                obj->spe = rne(3);

            /* Heavily relies on the fact that 1) we create wands
             * before rings, 2) that we create rings before
             * spellbooks, and that 3) not more than 1 object of a
             * particular symbol is to be prohibited.  (For more
             * objects, we need more nocreate variables...)
             */
            switch (otyp) {
            case WAN_POLYMORPH:
            case RIN_POLYMORPH:
            case POT_POLYMORPH:
                nocreate = RIN_POLYMORPH_CONTROL;
                break;
            case RIN_POLYMORPH_CONTROL:
                nocreate = RIN_POLYMORPH;
                nocreate2 = SPE_POLYMORPH;
                nocreate3 = POT_POLYMORPH;
            }
            /* Don't have 2 of the same ring or spellbook */
            if (obj->oclass == RING_CLASS || obj->oclass == SPBOOK_CLASS)
                nocreate4 = otyp;
        }

        if (urace.malenum != PM_HUMAN && !Role_if(PM_PIRATE)) {
            /* substitute race-specific items; this used to be in
               the 'if (otyp != UNDEF_TYP) { }' block above, but then
               substitutions didn't occur for randomly generated items
               (particularly food) which have racial substitutes */
            for (i = 0; inv_subs[i].race_pm != NON_PM; ++i)
                if (inv_subs[i].race_pm == urace.malenum
                    && otyp == inv_subs[i].item_otyp) {
                    debugpline3("ini_inv: substituting %s for %s%s",
                                OBJ_NAME(objects[inv_subs[i].subs_otyp]),
                                (trop->trotyp == UNDEF_TYP) ? "random " : "",
                                OBJ_NAME(objects[otyp]));
                    otyp = obj->otyp = inv_subs[i].subs_otyp;
                    break;
                }
        }

        /* nudist gets no armor */
        if (u.uroleplay.nudist && obj->oclass == ARMOR_CLASS) {
            dealloc_obj(obj);
            trop++;
            continue;
        }

        if (trop->trclass == COIN_CLASS) {
            /* no "blessed" or "identified" money */
            obj->quan = u.umoney0;
        } else {
            if (objects[otyp].oc_uses_known)
                obj->known = 1;
            obj->dknown = obj->bknown = obj->rknown = 1;
            if (Is_container(obj) || obj->otyp == STATUE) {
                obj->cknown = obj->lknown = 1;
                obj->otrapped = 0;
            }
            obj->cursed = 0;
            if (obj->opoisoned && u.ualign.type != A_CHAOTIC)
                obj->opoisoned = 0;
            if (obj->oclass == WEAPON_CLASS || obj->oclass == TOOL_CLASS) {
                obj->quan = (long) trop->trquan;
                trop->trquan = 1;
            } else if (obj->oclass == GEM_CLASS && is_graystone(obj)
                       && obj->otyp != FLINT) {
                obj->quan = 1L;
            }
            if (trop->trspe != UNDEF_SPE)
                obj->spe = trop->trspe;
            if (trop->trbless != UNDEF_BLESS)
                obj->blessed = trop->trbless;
        }
        /* defined after setting otyp+quan + blessedness */
        obj->owt = weight(obj);
        obj = addinv(obj);

        /* Make the type known if necessary */
        if (OBJ_DESCR(objects[otyp]) && obj->known)
            discover_object(otyp, TRUE, FALSE);
        if (otyp == OIL_LAMP)
            discover_object(POT_OIL, TRUE, FALSE);

        if (obj->oclass == ARMOR_CLASS) {
            if (is_shield(obj) && !uarms && !(uwep && bimanual(uwep))) {
                setworn(obj, W_ARMS);
                /* Prior to 3.6.2 this used to unset uswapwep if it was set, but
                   wearing a shield doesn't prevent having an alternate
                   weapon ready to swap with the primary; just make sure we
                   aren't two-weaponing (academic; no one starts that way) */
                u.twoweap = FALSE;
            } else if (is_helmet(obj) && !uarmh)
                setworn(obj, W_ARMH);
            else if (is_gloves(obj) && !uarmg)
                setworn(obj, W_ARMG);
            else if (is_shirt(obj) && !uarmu)
                setworn(obj, W_ARMU);
            else if (is_cloak(obj) && !uarmc)
                setworn(obj, W_ARMC);
            else if (is_boots(obj) && !uarmf)
                setworn(obj, W_ARMF);
            else if (is_suit(obj) && !uarm)
                setworn(obj, W_ARM);
        }

        if (obj->oclass == WEAPON_CLASS || is_weptool(obj)
            || otyp == TIN_OPENER || otyp == FLINT || otyp == ROCK) {
            if (is_ammo(obj) || is_missile(obj)) {
                if (!uquiver)
                    setuqwep(obj);
            } else if (!uwep && (!uarms || !bimanual(obj))) {
                setuwep(obj);
            } else if (!uswapwep) {
                setuswapwep(obj);
            }
        }
        if (obj->oclass == SPBOOK_CLASS && obj->otyp != SPE_BLANK_PAPER)
            initialspell(obj);

#if !defined(PYRAMID_BUG) && !defined(MAC)
        if (--trop->trquan)
            continue; /* make a similar object */
#else
        if (trop->trquan) { /* check if zero first */
            --trop->trquan;
            if (trop->trquan)
                continue; /* make a similar object */
        }
#endif
        trop++;
    }
}

/*u_init.c*/
