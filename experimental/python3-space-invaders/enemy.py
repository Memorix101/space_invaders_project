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

    def update(self):

        self.hitbox = pygame.Rect(self.pos_x, self.pos_y, 32, 32)

        if self.pos_x >= self.startPos + 200:
            self.goLeft = True         

        if self.pos_x <= self.startPos - 40:
            self.goLeft = False

        if self.goLeft == False:
            self.pos_x= self.pos_x + 3
        else:
            self.pos_x = self.pos_x - 3

    def draw(self, screen):
        if self.alive == True:
            screen.blit(self.tex2d, pygame.Rect(self.pos_x, self.pos_y, 32, 32), pygame.Rect(0, 0, 32, 32))