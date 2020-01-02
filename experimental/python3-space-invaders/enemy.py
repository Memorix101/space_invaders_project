import pygame

class Enemy:
    
    alive = True

    def __init__(self, pos, tex2d, rowid, maxShootTime):
        self.pos = pos
        self.pos_x = pos[0]
        self.pos_y = pos[1]
        self.tex2d = tex2d
        self.rowid = rowid
        self.maxShootTime = maxShootTime
        self.startPos = rowid * 40
        self.goLeft = False
        self.hitbox = pygame.Rect(0, 0, 32, 32)
        self.animationSpeed = 0.2
        self.moveSpeed = 3
        self.currentFrame = 0

    def animator(self):
        self.currentFrame = self.currentFrame + self.animationSpeed
        clamp = max(min(self.currentFrame, 4), 0)
        self.currentFrame = clamp
        
        if self.currentFrame >= 4:
            self.currentFrame = 0

    def update(self):

        self.animator()
        self.hitbox = pygame.Rect(self.pos_x, self.pos_y, 32, 32)

        if self.pos_x >= self.startPos + 200:
            self.goLeft = True         

        if self.pos_x <= self.startPos - 40:
            self.goLeft = False

        if self.goLeft == False:
            self.pos_x= self.pos_x + self.moveSpeed
        else:
            self.pos_x = self.pos_x - self.moveSpeed

    def draw(self, screen):
        if self.alive == True:
            screen.blit(self.tex2d, pygame.Rect(self.pos_x, self.pos_y, 32, 32), pygame.Rect(int(self.currentFrame) * 32, 0, 32, 32))