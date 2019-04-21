var config = {
    type: Phaser.AUTO,
    width: 640,
    height: 480,
    useTicker: true,
    physics: {
        default: 'arcade',
        arcade: {
            //gravity: { y: 200 }
        }
    },
    scale: {
        mode: Phaser.Scale.FIT,
        autoCenter: Phaser.Scale.CENTER_BOTH
    },
    scene: {
        init: init,
        preload: preload,
        create: create,
        update: update
    }
};

//game setuo
var game = new Phaser.Game(config);
var score = 0;
var scoreLabel = "SCORE: ";
var gameover = false;
var timedEvent;

//gameobjects
var player;
var isDead = false;
var laser;
var lasers; //group
var enemy_laser;
var enemy_lasers; //group
var enemy;
var enemies; //group
var explode;
var explosions; //group
var scoreText;
var text;
var deltaTime = 0;
var win_ui;
var gameover_ui;
var gameTimer = 0;
var fmg_splash;

//sound
var music;
var snd_blaster;
var snd_pusher;
var snd_explo;

//input manager
var key_spacebar;
var key_enter;
var key_a;
var key_d;
var arrow_keys;

function init() {
    //  Default text with no style settings
    //  Inject our CSS
    var element = document.createElement('style');
    document.head.appendChild(element);
    var sheet = element.sheet;
    var styles = '@font-face { font-family: "vermin"; src: url("rd/vermin_vibes_1989.otf") format("opentype"); }\n';
    sheet.insertRule(styles, 0);
}

function preload() {
    //this.load.setBaseURL('https://raw.githubusercontent.com/Memorix101/space_invaders_project/master/sdl2-cpp-space-invaders-master');
    this.load.image('space', 'rd/space3.png');
    this.load.image('logo', 'rd/phaser3-logo-nav.png');
    this.load.image('player', 'rd/player.png');
    this.load.image('laser', 'rd/bullet.png');
    this.load.image('enemy-laser', 'rd/enemy-bullet.png');
    this.load.image('win_ui', 'rd/win_ui.png');
    this.load.image('gameover_ui', 'rd/gameover_ui.png');
    this.load.image('fmg_splash', 'rd/fmg_splash.png');
    this.load.spritesheet('enemy', 'rd/invader32x32x4.png', {frameWidth: 32, frameHeight: 32});
    this.load.spritesheet('explode', 'rd/explode.png', {frameWidth: 128, frameHeight: 128});
    this.load.audio('music', 'rd/bodenstaendig.ogg');
    this.load.audio('snd_blaster', 'rd/blaster.ogg');
    this.load.audio('snd_pusher', 'rd/pusher.ogg');
    this.load.audio('snd_explo', 'rd/explode1.ogg');
}

function convertPad(s) {
    var str = "" + s;
    var convert = "0000";
    return scoreString = convert.substring(0, convert.length - str.length) + str;
}

function initGame() {
    gameover = false;
    isDead = false;
    score = 0;
    enemies.clear();
    lasers.clear();
    enemy_lasers.clear();
    music.stop(); //hmm, otherwise it will play alongside the "old" music source
}

function create() {
    //audio
    music = this.sound.add('music');
    if (!music.isPlaying) {
        music.play();
        music.loop = true;
    }
    snd_blaster = this.sound.add('snd_blaster');
    snd_pusher = this.sound.add('snd_pusher');
    snd_explo = this.sound.add('snd_explo');

    //input manager
    key_spacebar = this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.SPACE);
    key_enter = this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.ENTER);
    key_a = this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.A);
    key_d = this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.D);
    arrow_keys = this.input.keyboard.createCursorKeys();

    //enemies
    var enemyAnimation = this.anims.create({  //  Show the whole animation sheet
        key: 'walk',
        frames: this.anims.generateFrameNumbers('enemy'),
        frameRate: 15,
        repeat: -1 // looped animation
    });

    enemies = this.physics.add.group();
    for (var i = 0; i < 40; i++) {
        enemy = enemies.create(40, 100, 'enemy');
        enemy.play('walk', 0);
    }
    Phaser.Actions.GridAlign(enemies.getChildren(), {width: 10, cellWidth: 40, cellHeight: 40, x: 40, y: 80});

    enemies.getChildren().forEach(function (e) {
        var ran = Math.floor(Math.random() * 30) + 3; // 3 to 30
        e.spawnTimer = ran;
        e.timer = 0;
        //console.log(Math.floor(Math.random() * 100) + 1);
        this.tweens.add({
            targets: e,
            x: e.x + 200,
            ease: 'Sine.easeInOut',
            duration: 1500,
            flipX: false,
            yoyo: true,
            repeat: -1
        });
    }, this);

    //enemies
    var explodeAnimation = this.anims.create({  //  Show the whole animation sheet
        key: 'explode',
        frames: this.anims.generateFrameNumbers('explode'),
        frameRate: 15,
        //repeat: -1 // looped animation
        hideOnComplete: true
    });
    explosions = this.add.group();

    //player
    player = this.physics.add.image(400, 300, 'player')
        .setGravityY(0)
        .setCollideWorldBounds(true)
        .setPosition(320, 460);
    player.depth = 2;

    //lasers
    lasers = this.physics.add.group();
    this.physics.add.overlap(lasers, enemies, onHitEnemies, null, this);  //  When the laser sprite hits the enemy sprites, call this function ...

    // enemy lasers
    enemy_lasers = this.physics.add.group();
    this.physics.add.overlap(player, enemy_lasers, onHitPlayer, null, this);  //  When the enemy laser sprite hits the player sprite, call this function ...

    //game
    timedEvent = this.time.addEvent({delay: 500, loop: true});
    //timedEvent = this.time.addEvent({ delay: 500, callback: onEvent, callbackScope: this, loop: true });
    this.add.image(400, 300, 'space').depth = -5;
    fmg_splash = this.add.image(640 / 2, 480 / 2, 'fmg_splash');
    fmg_splash.depth = 5;
    win_ui = this.add.image(640 / 2, 480 / 2, 'win_ui');
    win_ui.visible = false;
    gameover_ui = this.add.image(640 / 2, 480 / 2 - 10, 'gameover_ui');
    gameover_ui.visible = false;
    scoreText = this.add.text(480, 10, scoreLabel.concat(convertPad(score.toString())), {
        fontFamily: 'vermin',
        fontSize: 24,
        color: '#ffffff'
    });

    text = this.add.text(32, 32);
    text = this.add.text(32, 32);

    this.add.image(400, 300, 'logo')
        .setAlpha(0.2)
        .setScale(0.5, 0.5)
        .setPosition(580, 460);
}

function enemy_update() {
    enemies.getChildren().forEach(function (e) {
        e.timer += deltaTime;
        //console.log(e.timer);
        if (e.timer >= e.spawnTimer) {
            e.timer = 0;
            //console.log("pew pew!");
            enemy_laser = enemy_lasers.create(e.x, e.y + 9, 'enemy-laser')
                .body.velocity.y = 250;
            enemy_lasers.setDepth(-3);
            snd_pusher.play();
        }
        //console.log(Math.floor(Math.random() * 100) + 1);
    }, this);
}

function spawnExplosion(x, y) {
    explosions.create(x, y, 'explode')
        .play('explode', 0);
    explosions.setDepth(-3);
}

function onHitEnemies(l, e) {
    e.disableBody(true, true);
    e.destroy();
    l.disableBody(true, true);
    spawnExplosion(e.x, e.y);
    snd_explo.play();
    score += 100;
    //console.log(e.name);
}

function onHitPlayer(p, e) {
    //p.body.enable = false; //  And disable the body
    p.disableBody(true, true);
    //enemy_lasers.killAndHide(e); //  Hide the sprite
    //e.body.enable = false; //  And disable the body
    //e.disableBody(true, true);
    spawnExplosion(p.x, p.y);
    snd_explo.play();
    isDead = true;
}

function update(time, delta) {
    if(gameTimer >= 3.0) {
        fmg_splash.visible = false;
        player.setVelocity(0);
        enemy_update();

        if (arrow_keys.left.isDown || key_a.isDown) {
            player.setVelocityX(-300);
        } else if (arrow_keys.right.isDown || key_d.isDown) {
            player.setVelocityX(300);
        }

        if (Phaser.Input.Keyboard.JustDown(key_spacebar) && !isDead) {
            laser = lasers.create(player.x, player.y - 18, 'laser')
                .body.velocity.y = -250;
            lasers.setDepth(-3);
            snd_blaster.play();
        }

        if (Phaser.Input.Keyboard.JustDown(key_enter) && gameover) {
            initGame();
            this.scene.restart();
        }

        lasers.getChildren().forEach(function (l) {
            if (l.y <= 0) {
                //console.log("destroy laser");
                l.destroy();
            }
        }, this);

        enemy_lasers.getChildren().forEach(function (l) {
            if (l.y >= 480) {
                //console.log("destroy enemy laser");
                l.destroy();
            }
        }, this);

        if (enemies.getLength() <= 0) {
            //console.log("You win!");
            gameover = true;
            win_ui.visible = true;
        } else {
            win_ui.visible = false;
        }

        if (isDead) {
            gameover = true;
            gameover_ui.visible = true;
        } else {
            gameover_ui.visible = false;
        }

        scoreText.text = scoreLabel.concat(convertPad(score.toString()));
        scoreText.depth = 100;
    }

    deltaTime = delta / 1000;
    gameTimer += deltaTime;
    //text.setText('Event.progress: ' + gameTimer);
}