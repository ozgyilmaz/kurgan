#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "interp.h"
#include "recycle.h"

DECLARE_MPROG_FUN_GREET( greet_prog_store	);
DECLARE_MPROG_FUN_GREET( greet_prog_weapon_shop	);
DECLARE_MPROG_FUN_GREET( greet_prog_food_shop	);
DECLARE_MPROG_FUN_GREET( greet_prog_magic_shop	);
DECLARE_MPROG_FUN_GREET( greet_prog_armoury	);
DECLARE_MPROG_FUN_GREET( greet_prog_healer	);
DECLARE_MPROG_FUN_GREET( greet_prog_map_shop	);
DECLARE_MPROG_FUN_GREET( greet_prog_jewelery	);
DECLARE_MPROG_FUN_GREET( greet_prog_bank_identifier	);
DECLARE_MPROG_FUN_GREET( greet_prog_questor	);

void mprog_set(MOB_INDEX_DATA *mobindex,const char *progtype, const char *name)
{
  if (!str_cmp(progtype, "greet_prog"))
  {
    if (!str_cmp(name, "greet_prog_store"))
      mobindex->mprogs->greet_prog = greet_prog_store;
    else if (!str_cmp(name, "greet_prog_weapon_shop"))
      mobindex->mprogs->greet_prog = greet_prog_weapon_shop;
    else if (!str_cmp(name, "greet_prog_food_shop"))
      mobindex->mprogs->greet_prog = greet_prog_food_shop;
    else if (!str_cmp(name, "greet_prog_magic_shop"))
      mobindex->mprogs->greet_prog = greet_prog_magic_shop;
    else if (!str_cmp(name, "greet_prog_armoury"))
      mobindex->mprogs->greet_prog = greet_prog_armoury;
    else if (!str_cmp(name, "greet_prog_healer"))
      mobindex->mprogs->greet_prog = greet_prog_healer;
    else if (!str_cmp(name, "greet_prog_map_shop"))
      mobindex->mprogs->greet_prog = greet_prog_map_shop;
    else if (!str_cmp(name, "greet_prog_jewelery"))
      mobindex->mprogs->greet_prog = greet_prog_jewelery;
    else if (!str_cmp(name, "greet_prog_bank_identifier"))
      mobindex->mprogs->greet_prog = greet_prog_bank_identifier;
    else if (!str_cmp(name, "greet_prog_questor"))
      mobindex->mprogs->greet_prog = greet_prog_questor;
    else
    {
      bug("Load_mprogs: 'M': Function not found for vnum %d",
      mobindex->vnum);
      exit(1);
    }

    SET_BIT(mobindex->progtypes, MPROG_GREET);
    return;
  }
  bug( "Load_mprogs: 'M': invalid program type for vnum %d",mobindex->vnum);
  exit(1);
 }

void greet_prog_store(CHAR_DATA *mob, CHAR_DATA *ch)
{
    if(number_range(1,100) <= 50)
      return;

    char buf[MAX_STRING_LENGTH];

    if (!can_see(mob,ch) || IS_NPC(ch) || IS_IMMORTAL(ch))
    {
        return;
    }

    interpret(mob,"smile");

    switch(number_range(1,20))
    {
      case 1:
        sprintf(buf, "Welcome to my shop, %s! Let me know if anything catches your eye.", ch->name);
        break;
      case 2:
        sprintf(buf, "Ah, %s! A fine day to trade, isn't it?", ch->name);
        break;
      case 3:
        sprintf(buf, "Greetings, %s. Take a look — finest wares in town!", ch->name);
        break;
      case 4:
        sprintf(buf, "Well met, %s. Looking for something special?", ch->name);
        break;
      case 5:
        sprintf(buf, "Good to see you, %s. My goods are yours to browse.", ch->name);
        break;
      case 6:
        sprintf(buf, "Step right in, %s! Fresh stock just arrived.", ch->name);
        break;
      case 7:
        sprintf(buf, "Hello there, %s. Need weapons, potions, or curiosities?", ch->name);
        break;
      case 8:
        sprintf(buf, "Ah, %s, welcome back! Still looking for the perfect item?", ch->name);
        break;
      case 9:
        sprintf(buf, "Hello, %s. I've got something that might interest you.", ch->name);
        break;
      case 10:
        sprintf(buf, "Hey, %s! Best deals this side of the realm!", ch->name);
        break;
      case 11:
        sprintf(buf, "Looking for rare goods? You've come to the right place!");
      case 12:
        sprintf(buf, "Don’t touch unless you’re buying... or unless you’re rich!");
      case 13:
        sprintf(buf, "Need something sharp, shiny, or suspicious?");
      case 14:
        sprintf(buf, "Everything here is... mostly legal. Mostly.");
      case 15:
        sprintf(buf, "I guarantee you won’t find better prices. Or your money back! (Not really.)");
      case 16:
        sprintf(buf, "I’ve got what you need — even if you don’t know it yet!");
      case 17:
        sprintf(buf, "Come closer, I won’t bite. The merchandise might.");
      case 18:
        sprintf(buf, "New in town? Equip yourself before it’s too late!");
      case 19:
        sprintf(buf, "Step in, friend. Coin speaks louder than words." );
      case 20:
        sprintf(buf, "My shop’s open, my secrets are not." );
      default:
        sprintf(buf,"Welcome to my shop, %s", ch->name );
        break;
    }
    do_say(mob,buf);
}

void greet_prog_weapon_shop(CHAR_DATA *mob, CHAR_DATA *ch)
{
    if(number_range(1,100) <= 50)
      return;

    char buf[MAX_STRING_LENGTH];

    if (!can_see(mob,ch) || IS_NPC(ch) || IS_IMMORTAL(ch))
    {
        return;
    }

    interpret(mob,"smile");

    switch(number_range(1,20))
    {
      case 1:
        sprintf(buf, "Ah, %s! You look like someone who appreciates fine steel.", ch->name);
        break;
      case 2:
        sprintf(buf, "Welcome, %s. Care to upgrade that rusty blade?", ch->name);
        break;
      case 3:
        sprintf(buf, "Well met, %s. A sharper edge could save your life.", ch->name);
        break;
      case 4:
        sprintf(buf, "Greetings, %s. I've got weapons fit for a warlord.", ch->name);
        break;
      case 5:
        sprintf(buf, "Step in, %s. I've got blades that thirst for battle.", ch->name);
        break;
      case 6:
        sprintf(buf, "Hello there, %s. Your enemies won't know what hit 'em.", ch->name);
        break;
      case 7:
        sprintf(buf, "Hey %s, looking to trade up to something... deadlier?", ch->name);
        break;
      case 8:
        sprintf(buf, "Back again, %s? Worn out your sword already?", ch->name);
        break;
      case 9:
        sprintf(buf, "Hail, %s. Every weapon here has drawn blood — yours could be next.", ch->name);
        break;
      case 10:
        sprintf(buf, "Welcome, %s. I sell power — forged in fire and blood.", ch->name);
        break;
      case 11:
        sprintf(buf, "Steel, iron, obsidian... what’s your poison?" );
      case 12:
        sprintf(buf, "A good weapon makes the difference between a story and a grave.");
      case 13:
        sprintf(buf, "Touch the blade if you dare — it’s still warm from its last kill.");
      case 14:
        sprintf(buf, "Looking for something that bites harder than a dragon?" );
      case 15:
        sprintf(buf, "These aren't toys. These are tools of survival.");
      case 16:
        sprintf(buf, "Every blade here has a story... some are still being written.");
      case 17:
        sprintf(buf, "Going somewhere dangerous? You’ll want to be properly armed.");
      case 18:
        sprintf(buf, "You bring coin, I bring carnage.");
      case 19:
        sprintf(buf, "No refunds. Dead men rarely complain." );
      case 20:
        sprintf(buf, "If you break it, you buy it. If it breaks you... well, too bad." );
      default:
        sprintf(buf,"Welcome to my shop, %s", ch->name );
        break;
    }
    do_say(mob,buf);
}

void greet_prog_food_shop(CHAR_DATA *mob, CHAR_DATA *ch)
{
    if(number_range(1,100) <= 50)
      return;
    
    char buf[MAX_STRING_LENGTH];

    if (!can_see(mob,ch) || IS_NPC(ch) || IS_IMMORTAL(ch))
    {
        return;
    }

    interpret(mob,"smile");

    switch(number_range(1,20))
    {
      case 1:
        sprintf(buf, "Ah, %s! You’ve come at the perfect time — the bread is still warm!", ch->name);
        break;
      case 2:
        sprintf(buf, "Welcome, %s. Care for a sweetroll or something more savory?", ch->name);
        break;
      case 3:
        sprintf(buf, "Good day, %s. Fresh pies just came out of the oven!", ch->name);
        break;
      case 4:
        sprintf(buf, "Greetings, %s! The smell of cinnamon and hope awaits you.", ch->name);
        break;
      case 5:
        sprintf(buf, "Step inside, %s. Hunger is the only monster we fight here.", ch->name);
        break;
      case 6:
        sprintf(buf, "Hello, %s. Got a craving? We’ve got solutions.", ch->name);
        break;
      case 7:
        sprintf(buf, "Hey %s! One bite of my pastries and you'll never leave.", ch->name);
        break;
      case 8:
        sprintf(buf, "Back again, %s? Couldn’t resist the smell, could you?", ch->name);
        break;
      case 9:
        sprintf(buf, "Hail, %s. We’ve got loaves, laughter, and a little bit of magic.", ch->name);
        break;
      case 10:
        sprintf(buf, "Welcome, %s. Feed your belly and your soul.", ch->name);
        break;
      case 11:
        sprintf(buf, "Fresh bread, warm hearts — step right in!");
        break;
      case 12:
        sprintf(buf, "Hungry traveler? We’ve got everything from stew to sweets.");
        break;
      case 13:
        sprintf(buf, "Smells good, doesn’t it? That’s the scent of happiness.");
        break;
      case 14:
        sprintf(buf, "Nothing beats a hot pie on a cold day.");
        break;
      case 15:
        sprintf(buf, "You break fast, I break bread. Let’s talk.");
        break;
      case 16:
        sprintf(buf, "We don’t just sell food — we serve comfort.");
        break;
      case 17:
        sprintf(buf, "No weapons here — just warm meals and warmer smiles.");
        break;
      case 18:
        sprintf(buf, "Our pastries have been known to bring people back from the brink.");
        break;
      case 19:
        sprintf(buf, "If it smells too good to be true — it’s probably from my oven.");
        break;
      case 20:
        sprintf(buf, "Buy a loaf, get a smile. Buy two, get a secret recipe.");
        break;
      default:
        sprintf(buf,"Welcome to my bakery, %s!", ch->name);
        break;
    }

    do_say(mob,buf);
}

void greet_prog_magic_shop(CHAR_DATA *mob, CHAR_DATA *ch)
{
    if(number_range(1,100) <= 50)
      return;

    char buf[MAX_STRING_LENGTH];

    if (!can_see(mob,ch) || IS_NPC(ch) || IS_IMMORTAL(ch))
    {
        return;
    }

    interpret(mob,"nod");

    switch(number_range(1,20))
    {
      case 1:
        sprintf(buf, "Ah, %s... I sensed your arrival in the smoke just moments ago.", ch->name);
        break;
      case 2:
        sprintf(buf, "Welcome, %s. Looking for potions, scrolls, or perhaps... forbidden knowledge?", ch->name);
        break;
      case 3:
        sprintf(buf, "Well met, %s. The stars told me you'd come today.", ch->name);
        break;
      case 4:
        sprintf(buf, "Greetings, %s. The arcane winds are strong — a good day to shop.", ch->name);
        break;
      case 5:
        sprintf(buf, "Step lightly, %s. Some of the items here are... unstable.", ch->name);
        break;
      case 6:
        sprintf(buf, "Hello, %s. Mind the glowing orb. It bites.", ch->name);
        break;
      case 7:
        sprintf(buf, "Ah, %s! Come to trade gold for secrets, have you?", ch->name);
        break;
      case 8:
        sprintf(buf, "Back again, %s? Careful — the last wand you bought still hasn't cooled down.", ch->name);
        break;
      case 9:
        sprintf(buf, "Hail, %s. The veil between worlds is thin today... choose wisely.", ch->name);
        break;
      case 10:
        sprintf(buf, "Welcome, %s. Magic is no toy... unless you're very rich.", ch->name);
        break;
      case 11:
        sprintf(buf, "Do not touch the crystal. It remembers.");
        break;
      case 12:
        sprintf(buf, "Eyes open, tongue shut. Some scrolls read back.");
        break;
      case 13:
        sprintf(buf, "We deal in power, mystery, and incense. Mostly incense.");
        break;
      case 14:
        sprintf(buf, "Looking for something enchanted? Or... cursed?");
        break;
      case 15:
        sprintf(buf, "I assure you — every item here has a story. Some end well.");
        break;
      case 16:
        sprintf(buf, "I have potions that heal, reveal, and occasionally explode.");
        break;
      case 17:
        sprintf(buf, "No refunds. Magic doesn’t care about your regrets.");
        break;
      case 18:
        sprintf(buf, "The candle flickers — you must be a sorcerer. Or very unlucky.");
        break;
      case 19:
        sprintf(buf, "Be careful what you buy... some of it buys back.");
        break;
      case 20:
        sprintf(buf, "Choose fast, the runes are shifting again.");
        break;
      default:
        sprintf(buf,"Welcome to my arcane emporium, %s.", ch->name);
        break;
    }

    do_say(mob,buf);
}

void greet_prog_armoury(CHAR_DATA *mob, CHAR_DATA *ch)
{
    if(number_range(1,100) <= 50)
      return;

    char buf[MAX_STRING_LENGTH];

    if (!can_see(mob,ch) || IS_NPC(ch) || IS_IMMORTAL(ch))
    {
        return;
    }

    interpret(mob,"nod");

    switch(number_range(1,20))
    {
      case 1:
        sprintf(buf, "Ah, %s! You look like someone who values staying alive.", ch->name);
        break;
      case 2:
        sprintf(buf, "Welcome, %s. Strong armor is the quiet guardian of every victory.", ch->name);
        break;
      case 3:
        sprintf(buf, "Well met, %s. A new breastplate could be the difference between legend and memory.", ch->name);
        break;
      case 4:
        sprintf(buf, "Greetings, %s. May your enemies break against your armor like waves on stone.", ch->name);
        break;
      case 5:
        sprintf(buf, "Step in, %s. My shields hold truer than any oath.", ch->name);
        break;
      case 6:
        sprintf(buf, "Hello, %s. Chainmail, leather, or plate — I have it all.", ch->name);
        break;
      case 7:
        sprintf(buf, "Hey %s! Your armor looks tired. Let's fix that.", ch->name);
        break;
      case 8:
        sprintf(buf, "Back again, %s? That dented helm didn’t hold, did it?", ch->name);
        break;
      case 9:
        sprintf(buf, "Hail, %s. Armor up — caution is the companion of warriors.", ch->name);
        break;
      case 10:
        sprintf(buf, "Welcome, %s. If it’s protection you need, you’ve come to the right place.", ch->name);
        break;
      case 11:
        sprintf(buf, "Good armor doesn’t shine — it *survives*.");
        break;
      case 12:
        sprintf(buf, "Steel, leather, mythril — everything forged to keep you standing.");
        break;
      case 13:
        sprintf(buf, "I don’t sell fashion, I sell peace of mind.");
        break;
      case 14:
        sprintf(buf, "Battle’s coming, friend. Be ready — don’t skimp on armor.");
        break;
      case 15:
        sprintf(buf, "Every scar tells a story. Good armor helps you live to tell it.");
        break;
      case 16:
        sprintf(buf, "Light armor for speed, heavy for safety. What's your style?");
        break;
      case 17:
        sprintf(buf, "No refunds. If it cracks, you’re probably dead anyway.");
        break;
      case 18:
        sprintf(buf, "Armor isn’t for the fearless. It’s for the wise.");
        break;
      case 19:
        sprintf(buf, "The best offense is a good defense. And a better helmet.");
        break;
      case 20:
        sprintf(buf, "If you're going to face dragons, wear something stronger than hope.");
        break;
      default:
        sprintf(buf, "Welcome to my armoury, %s.", ch->name);
        break;
    }

    do_say(mob, buf);
}

void greet_prog_healer(CHAR_DATA *mob, CHAR_DATA *ch)
{
    if(number_range(1,100) <= 50)
      return;

    char buf[MAX_STRING_LENGTH];

    if (!can_see(mob,ch) || IS_NPC(ch) || IS_IMMORTAL(ch))
    {
        return;
    }

    interpret(mob,"smile");

    switch(number_range(1,20))
    {
      case 1:
        sprintf(buf, "Ah, %s... You carry pain in your step. Come, let us ease it.", ch->name);
        break;
      case 2:
        sprintf(buf, "Welcome, %s. Healing is not just for the body, but for the soul.", ch->name);
        break;
      case 3:
        sprintf(buf, "Well met, %s. I sense wounds—visible or not.", ch->name);
        break;
      case 4:
        sprintf(buf, "Greetings, %s. The light guides all who seek relief.", ch->name);
        break;
      case 5:
        sprintf(buf, "Step softly, %s. This is a place of peace and mending.", ch->name);
        break;
      case 6:
        sprintf(buf, "Hello, %s. Do you need salves, potions, or simply rest?", ch->name);
        break;
      case 7:
        sprintf(buf, "Hey %s, sit. You look like you've fought shadows lately.", ch->name);
        break;
      case 8:
        sprintf(buf, "Back again, %s? Even the brave need care.", ch->name);
        break;
      case 9:
        sprintf(buf, "Hail, %s. The gods watch over you — and so do I.", ch->name);
        break;
      case 10:
        sprintf(buf, "Welcome, %s. Healing is never far for those who believe.", ch->name);
        break;
      case 11:
        sprintf(buf, "Let go of pain. Breathe in the calm.");
        break;
      case 12:
        sprintf(buf, "Not all scars are visible. But all can be healed.");
        break;
      case 13:
        sprintf(buf, "These hands have mended kings and beggars alike.");
        break;
      case 14:
        sprintf(buf, "The herbs speak, the light listens — and I heal.");
        break;
      case 15:
        sprintf(buf, "Come wounded, leave whole. That is our way.");
        break;
      case 16:
        sprintf(buf, "No battle too fierce, no wound too deep.");
        break;
      case 17:
        sprintf(buf, "Faith, rest, and a bit of willowbark — the healer's magic.");
        break;
      case 18:
        sprintf(buf, "A quiet heart heals faster than a restless one.");
        break;
      case 19:
        sprintf(buf, "Do not fear. Healing begins the moment you ask for it.");
        break;
      case 20:
        sprintf(buf, "I offer peace for the body, balm for the mind.");
        break;
      default:
        sprintf(buf, "Welcome to my sanctuary, %s.", ch->name);
        break;
    }

    do_say(mob, buf);
}

void greet_prog_map_shop(CHAR_DATA *mob, CHAR_DATA *ch)
{
    if(number_range(1,100) <= 50)
      return;

    char buf[MAX_STRING_LENGTH];

    if (!can_see(mob,ch) || IS_NPC(ch) || IS_IMMORTAL(ch))
    {
        return;
    }

    interpret(mob,"smile");

    switch(number_range(1,20))
    {
      case 1:
        sprintf(buf, "Ah, %s! A traveler’s heart, I can tell. Come — I have maps for every dream.", ch->name);
        break;
      case 2:
        sprintf(buf, "Welcome, %s. Seeking treasure, danger, or something in between?", ch->name);
        break;
      case 3:
        sprintf(buf, "Well met, %s. Every line on a map is a story waiting to unfold.", ch->name);
        break;
      case 4:
        sprintf(buf, "Greetings, %s. I’ve charted lands no bard dares to sing of.", ch->name);
        break;
      case 5:
        sprintf(buf, "Step in, %s. The world is vast — and I’ve drawn most of it.", ch->name);
        break;
      case 6:
        sprintf(buf, "Hello, %s. Looking for the shortest path to glory? Or the longest to peace?", ch->name);
        break;
      case 7:
        sprintf(buf, "Hey %s! The desert route? The haunted vale? Name your madness.", ch->name);
        break;
      case 8:
        sprintf(buf, "Back again, %s? You’ve got the eyes of someone who’s been off the edge.", ch->name);
        break;
      case 9:
        sprintf(buf, "Hail, %s. The road ahead is perilous — better take a map.", ch->name);
        break;
      case 10:
        sprintf(buf, "Welcome, %s. I trade in ink, parchment, and the promise of direction.", ch->name);
        break;
      case 11:
        sprintf(buf, "The world is bigger than you think. Luckily, I make it pocket-sized.");
        break;
      case 12:
        sprintf(buf, "No one ever got lost with one of my maps… except that one guy.");
        break;
      case 13:
        sprintf(buf, "Mountains, ruins, forgotten trails — I’ve seen them all. On paper, anyway.");
        break;
      case 14:
        sprintf(buf, "A good map is like magic. It shows what others cannot see.");
        break;
      case 15:
        sprintf(buf, "Beware: some maps don’t just lead you places… they pull you in.");
        break;
      case 16:
        sprintf(buf, "Every fold, every mark — drawn from whispers, not just memory.");
        break;
      case 17:
        sprintf(buf, "I don’t just sell maps. I sell direction. Destiny, even.");
        break;
      case 18:
        sprintf(buf, "Some say the world ends at the edge of the map. I say: draw further.");
        break;
      case 19:
        sprintf(buf, "Don’t trust old legends — trust fresh parchment.");
        break;
      case 20:
        sprintf(buf, "Ink fades, but adventure doesn’t. Take one with you.");
        break;
      default:
        sprintf(buf, "Welcome to my chart room, %s.", ch->name);
        break;
    }

    do_say(mob, buf);
}

void greet_prog_jewelery(CHAR_DATA *mob, CHAR_DATA *ch)
{
    if(number_range(1,100) <= 50)
      return;

    char buf[MAX_STRING_LENGTH];

    if (!can_see(mob,ch) || IS_NPC(ch) || IS_IMMORTAL(ch))
    {
        return;
    }

    interpret(mob,"smile");

    switch(number_range(1,20))
    {
      case 1:
        sprintf(buf, "Ah, %s! You shine bright — but a little gemstone never hurts.", ch->name);
        break;
      case 2:
        sprintf(buf, "Welcome, %s. Looking to dazzle? I've got just the sparkle.", ch->name);
        break;
      case 3:
        sprintf(buf, "Well met, %s. For beauty, for power, or for love?", ch->name);
        break;
      case 4:
        sprintf(buf, "Greetings, %s. Each gem here holds a secret... and a price.", ch->name);
        break;
      case 5:
        sprintf(buf, "Step in, %s. Let’s find something that reflects your soul — or hides it.", ch->name);
        break;
      case 6:
        sprintf(buf, "Hello, %s. My finest rings whisper to those who truly listen.", ch->name);
        break;
      case 7:
        sprintf(buf, "Hey %s! What’s more dangerous — a blade, or a glance wearing emeralds?", ch->name);
        break;
      case 8:
        sprintf(buf, "Back again, %s? Love trouble? Amulet of charm, maybe?", ch->name);
        break;
      case 9:
        sprintf(buf, "Hail, %s. A gem for every desire — even the unspoken ones.", ch->name);
        break;
      case 10:
        sprintf(buf, "Welcome, %s. Want to shine like royalty? I’ve got what they *wish* they had.", ch->name);
        break;
      case 11:
        sprintf(buf, "A ruby for courage. A sapphire for wisdom. Or just something shiny?");
        break;
      case 12:
        sprintf(buf, "They say diamonds are forever. I say — only if paid in full.");
        break;
      case 13:
        sprintf(buf, "Some rings bind hearts. Others bind souls. Choose carefully.");
        break;
      case 14:
        sprintf(buf, "Jewels don’t lie. But they do tempt.");
        break;
      case 15:
        sprintf(buf, "Everything here is enchanted — with beauty, or worse.");
        break;
      case 16:
        sprintf(buf, "A simple pendant. A heavy crown. I sell both — and know their weight.");
        break;
      case 17:
        sprintf(buf, "Looking to impress? Or to hide power in plain sight?");
        break;
      case 18:
        sprintf(buf, "My gems have passed through royal hands — and bloody ones.");
        break;
      case 19:
        sprintf(buf, "You don’t wear jewelry. It wears *you*.");
        break;
      case 20:
        sprintf(buf, "Nothing draws eyes like a well-placed gem. Trust me.");
        break;
      default:
        sprintf(buf, "Welcome to my jewelry shop, %s.", ch->name);
        break;
    }

    do_say(mob, buf);
}

void greet_prog_bank_identifier(CHAR_DATA *mob, CHAR_DATA *ch)
{
    if(number_range(1,100) <= 50)
      return;

    char buf[MAX_STRING_LENGTH];

    if (!can_see(mob,ch) || IS_NPC(ch) || IS_IMMORTAL(ch))
    {
        return;
    }

    interpret(mob,"nod");

    switch(number_range(1,20))
    {
      case 1:
        sprintf(buf, "Ah, %s. Looking to deposit your fortune, or reveal another's?", ch->name);
        break;
      case 2:
        sprintf(buf, "Welcome, %s. Gold, secrets, or strange artifacts today?", ch->name);
        break;
      case 3:
        sprintf(buf, "Well met, %s. My vault is safe, and my eyes sharper than most.", ch->name);
        break;
      case 4:
        sprintf(buf, "Greetings, %s. Some items are worth more than their weight — shall we see?", ch->name);
        break;
      case 5:
        sprintf(buf, "Step in, %s. Coins sleep here. Truths awaken.", ch->name);
        break;
      case 6:
        sprintf(buf, "Hello, %s. Banking, or are we unlocking mysteries today?", ch->name);
        break;
      case 7:
        sprintf(buf, "Hey %s! That pouch looks heavy. The vault’s this way.", ch->name);
        break;
      case 8:
        sprintf(buf, "Back again, %s? Found another ring that hums when no one’s watching?", ch->name);
        break;
      case 9:
        sprintf(buf, "Hail, %s. Safe storage, safe mind. Or... dangerous knowledge?", ch->name);
        break;
      case 10:
        sprintf(buf, "Welcome, %s. I guard gold and unveil hidden powers. Choose wisely.", ch->name);
        break;
      case 11:
        sprintf(buf, "Not all valuables shine. Some whisper.");
        break;
      case 12:
        sprintf(buf, "Magic leaves traces. I know how to read them.");
        break;
      case 13:
        sprintf(buf, "Your coin is safe here. Safer than most kings, in fact.");
        break;
      case 14:
        sprintf(buf, "Want to know what that trinket really is? It might surprise you.");
        break;
      case 15:
        sprintf(buf, "Some items are cursed, some are priceless. I tell you which.");
        break;
      case 16:
        sprintf(buf, "The vault doesn’t judge. But I do — especially your boots.");
        break;
      case 17:
        sprintf(buf, "No one breaks into this bank. The last one’s still screaming — somewhere.");
        break;
      case 18:
        sprintf(buf, "A good bank keeps your wealth. A great one tells you what’s truly worth keeping.");
        break;
      case 19:
        sprintf(buf, "If it glows, floats, or whispers, I can name it — for a price.");
        break;
      case 20:
        sprintf(buf, "Money talks. But enchanted objects shout. Let's listen.");
        break;
      default:
        sprintf(buf, "Welcome to the vault and vision, %s.", ch->name);
        break;
    }

    do_say(mob, buf);
}

void greet_prog_questor(CHAR_DATA *mob, CHAR_DATA *ch)
{
    if(number_range(1,100) <= 50)
      return;
      
    char buf[MAX_STRING_LENGTH];

    if (!can_see(mob,ch) || IS_NPC(ch) || IS_IMMORTAL(ch))
    {
        return;
    }

    interpret(mob,"look");

    switch(number_range(1,20))
    {
      case 1:
        sprintf(buf, "Ah, %s! Just the kind of soul fate tends to nudge.", ch->name);
        break;
      case 2:
        sprintf(buf, "Welcome, %s. There's a matter that requires... a certain bravery.", ch->name);
        break;
      case 3:
        sprintf(buf, "Well met, %s. Are you here for coin, glory, or justice?", ch->name);
        break;
      case 4:
        sprintf(buf, "Greetings, %s. A shadow stirs in the East... will you answer?", ch->name);
        break;
      case 5:
        sprintf(buf, "Step closer, %s. I have something that may interest a wanderer such as you.", ch->name);
        break;
      case 6:
        sprintf(buf, "Hello, %s. You carry the look of someone searching for purpose.", ch->name);
        break;
      case 7:
        sprintf(buf, "Hey %s, you any good with a blade... or your wits?", ch->name);
        break;
      case 8:
        sprintf(buf, "Back again, %s? There’s always more that needs doing.", ch->name);
        break;
      case 9:
        sprintf(buf, "Hail, %s. I hope your feet aren't tired — adventure rarely waits.", ch->name);
        break;
      case 10:
        sprintf(buf, "Welcome, %s. I was told someone like you would come.", ch->name);
        break;
      case 11:
        sprintf(buf, "Not all heroes wear capes — some just arrive at the right time.");
        break;
      case 12:
        sprintf(buf, "A storm brews, and the people need hope. Or at least a sword arm.");
        break;
      case 13:
        sprintf(buf, "Some say destiny knocks. I prefer to whisper it to those who listen.");
        break;
      case 14:
        sprintf(buf, "Gold? Honor? Revenge? Whatever your reason, I may have a task for you.");
        break;
      case 15:
        sprintf(buf, "Too many stand by. Too few step up. Will you?");
        break;
      case 16:
        sprintf(buf, "No time for small talk — lives hang in the balance.");
        break;
      case 17:
        sprintf(buf, "You’ll want to hear this. It may change your path forever.");
        break;
      case 18:
        sprintf(buf, "This world has wounds, and you may be the one to heal them.");
        break;
      case 19:
        sprintf(buf, "Maps speak of places. I speak of deeds. Are you ready?");
        break;
      case 20:
        sprintf(buf, "If you came looking for adventure... it’s been looking for you, too.");
        break;
      default:
        sprintf(buf, "Welcome, %s. There is work to be done.", ch->name);
        break;
    }

    do_say(mob, buf);
}
