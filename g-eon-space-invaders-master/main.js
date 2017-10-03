// Memorix101

GAME_MODULE_INFO("Test", 0.86);

//load content
load_content("music", "bodenstaendig", "assets/bodenstaendig.mp3");
load_content("sfx", "blaster", "assets/blaster.ogg");
load_content("sfx", "pusher", "assets/pusher.wav");
load_content("sfx", "explode1", "assets/explode1.wav");
load_content("gfx", "player", "assets/player.png");
load_content("gfx", "fmg", "assets/fmg_splash.png");
load_content("gfx", "space3", "assets/space3.png");
load_content("gfx", "invader", "assets/invader32x32x4.png");
load_content("gfx", "bullet", "assets/bullet.png");
load_content("gfx", "enemy-bullet", "assets/enemy-bullet.png");
load_content("gfx", "explode", "assets/explode.png");
load_content("font", "vermin-vibes", "assets/vermin-vibes.ttf");
load_content("font", "score_ttf", "assets/vermin-vibes.ttf");

var myText = new font();
myText.load(25, "vermin-vibes");
myText.color(255, 0, 0, 255);

var score = 0;

function convertPad(s) {
    var str = "" + s;
    var convert = "0000";
    return scoreString = convert.substring(0, convert.length - str.length) + str;
}

var scoreTTF = new font();
scoreTTF.load(25, "score_ttf");
scoreTTF.color(255, 0, 0, 255);

var snd_blaster = new sfx();
snd_blaster.load("blaster");
snd_blaster.volume(100);
snd_blaster.setchannel(1);

var snd_pusher = new sfx();
snd_pusher.load("pusher");
snd_pusher.volume(100);
snd_pusher.setchannel(1);

var snd_explo = new sfx();
snd_explo.load("explode1");
snd_explo.volume(75);
snd_explo.setchannel(2);

var music_file = new music();
music_file.load("bodenstaendig");
music_file.volume(150);
music_file.play(true);

var fmg = new gfx();
fmg.load("fmg");

var space3 = new gfx();
space3.load("space3");

var player = {
    tex: new gfx(),
    hitbox: new rect(0, 0, 21, 28),
    pos: new vector2(640 / 2 - 28 / 2, 420), //start pos
    alive: true
};

player.tex.load("player");
player.tex.position(player.pos.x, player.pos.y); // start pos

var bulletList = [];

var enemyBulletsList = [];

var enemyList = [];
var enemyDrawRect = new rect(0, 0, 32, 32);
var rowCount = 0;
var itemCount = 0;

var exploList = [];
var exploDrawRect = new rect(0, 0, 128, 128);

for (var i = 0; i < 40; i++) {
    if (i % 10 == 0) {
        itemCount = 0;
        rowCount++;
    }
    itemCount++;
    enemyList.push({
        tex: new gfx(),
        pos: new vector2(0, 0),
        goLeft: false,
        startPos: 0,
        rowPosID: 0,
        currentFrame: 0,
        shootTimeLimit: Math.random() * (30 - 3) + 3, //Math.random() * (max - min) + min;
        shootTimer: 0,
        shoot: false,
        hitbox: new rect(0, 0, 32, 32),
        alive: true
    });
    enemyList[i].tex.load("invader");
    enemyList[i].pos.x = itemCount * 40;
    enemyList[i].pos.y = 40 * rowCount;
    enemyList[i].rowPosID = 40 * (11 - itemCount);
    enemyList[i].startPos = enemyList[i].pos.x;
    enemyList[i].tex.position(enemyList[i].pos.x, enemyList[i].pos.y); // start pos
}

var startGame = false;
var start = 0;
var a_button = false;
var a_button_down = false;

function Update() {
    start += 1 * deltatime;

    if (startGame) {

        if (button_down(0, LEFT_BUTTON)) {
            player.pos.x -= 350 * deltatime;
        }

        if (button_down(0, RIGHT_BUTTON)) {
            player.pos.x += 350 * deltatime;
        }

        if (button_down(0, A_BUTTON)) {
            a_button = true;
        } else {
            a_button = false;
            a_button_down = false;
        }

        n = maths_clamp(player.pos.x, 1, 640 - 26);
        player.tex.position(n, player.pos.y);

        player.hitbox.x = player.pos.x; //update hitbox x
        player.hitbox.y = player.pos.y; //update hitbox y

        if (a_button == true && a_button_down == false && player.alive) {
            bulletList.push({
                tex: new gfx(),
                pos: new vector2(player.pos.x + 9, player.pos.y - 13),
                hitbox: new rect(0, 0, 6, 36),
                alive: true
            });

            bulletList[bulletList.length - 1].tex.load("bullet");

            snd_blaster.play();
            a_button_down = true;
        }

        enemyList.forEach(function(element) {

            element.currentFrame += 20 * deltatime;

            if (element.currentFrame >= 4) {
                element.currentFrame = 0;
            }

            enemyDrawRect.x = Math.floor(element.currentFrame) * 32;

            if (!element.goLeft) {
                element.pos.x += 250 * deltatime;
            }

            if (element.goLeft) {
                element.pos.x -= 250 * deltatime;
            }

            if (element.pos.x >= 640 - (32 + element.rowPosID) && !element.goLeft) {
                element.goLeft = true;
            }

            if (element.pos.x <= element.startPos + 32 && element.goLeft) {
                element.goLeft = false;
            }

            element.shootTimer += 1 * deltatime; // Elapsed time since game start synced to 60 fps

            if (element.shootTimer >= element.shootTimeLimit) {
                element.shootTimer = 0;
                element.shoot = true;
            } else {
                element.shoot = false;
            }

            if (element.shoot && element.alive) {
                enemyBulletsList.push({
                    tex: new gfx(),
                    pos: new vector2(element.pos.x + 4, element.pos.y - 4),
                    hitbox: new rect(0, 0, 9, 9),
                    alive: true
                });

                enemyBulletsList[enemyBulletsList.length - 1].tex.load("enemy-bullet");

                snd_pusher.play();
            }

            if (element.alive) {
                element.tex.position(element.pos.x, element.pos.y); //update position
                element.hitbox.x = element.pos.x; //update hitbox x
                element.hitbox.y = element.pos.y; //update hitbox y
            }
        });

        bulletList.forEach(function(element) {

            if (element.alive) {
                element.tex.position(element.pos.x, element.pos.y -= 350 * deltatime); //update position and apply speed
                element.hitbox.x = element.pos.x; //update hitbox x
                element.hitbox.y = element.pos.y; //update hitbox y
            }

            if (element.pos.y < 0) {
                element.alive = false;
            }

        });

        enemyBulletsList.forEach(function(element) {

            if (element.alive) {
                element.tex.position(element.pos.x, element.pos.y += 350 * deltatime); //update position and apply speed
                element.hitbox.x = element.pos.x; //update hitbox x
                element.hitbox.y = element.pos.y; //update hitbox y
            }

            if (element.pos.y > 480) {
                element.alive = false;
            }

        });

        bulletList.forEach(function(bullet) {
            enemyList.forEach(function(enemy) {
                if (enemy.alive && bullet.alive) {
                    if (bullet.hitbox.intersect(enemy.hitbox)) {
                        print('BOOM!');
                        enemy.alive = false;
                        bullet.alive = false;

                        exploList.push({
                            tex: new gfx(),
                            currentFrame: 0,
                            alive: true
                        });

                        exploList[exploList.length - 1].tex.position(enemy.pos.x - 64 + 16, enemy.pos.y - 64 + 16);
                        exploList[exploList.length - 1].tex.load("explode");
                        snd_explo.play();
                        score += 100;
                    }
                }
            });
        });

        enemyBulletsList.forEach(function(enemy) {
            if (enemy.alive && player.alive) {
                if (player.hitbox.intersect(enemy.hitbox)) {
                    print('BOOM!');
                    enemy.alive = false;
                    player.alive = false;

                    exploList.push({
                        tex: new gfx(),
                        currentFrame: 0,
                        alive: true
                    });

                    exploList[exploList.length - 1].tex.position(player.pos.x - 21 - 32, player.pos.y - 28 - 32);
                    exploList[exploList.length - 1].tex.load("explode");
                    snd_explo.play();
                }
            }
        });

        exploList.forEach(function(element) {

            element.currentFrame += (1 / 60) * 15;

            exploDrawRect.x = Math.floor(element.currentFrame) * 128;

            if (Math.floor(element.currentFrame) >= 4) {
                element.alive = false;
            }

        });
    }
}

function Draw() {
    gfx_drawcolor(0, 0, 0, 0);

    gfx_clear();

    if (startGame) {
        space3.draw(null);

        myText.draw("SCORE: ", 480, 10);
        scoreTTF.draw(convertPad(score.toString()), 560, 10);

        bulletList.forEach(function(element) {
            if (element.alive && element.tex != null)
                element.tex.draw(null);
        });

        if (player.alive)
            player.tex.draw(null);

        enemyBulletsList.forEach(function(element) {
            if (element.alive && element.tex != null)
                element.tex.draw(null);
        });

        enemyList.forEach(function(element) {
            if (element.alive && element.tex != null)
                element.tex.draw(enemyDrawRect);
        });

        exploList.forEach(function(element) {
            if (element.alive && element.tex != null)
                element.tex.draw(exploDrawRect);
        });

    }
    if (start < 3) // three seconds
    {
        fmg.draw(null);
    } else {
        startGame = true;
    }

    gfx_flip();
}
