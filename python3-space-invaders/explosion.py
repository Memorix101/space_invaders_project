import pygame

class Explosion:
    def __init__(self, pos, tex2d):
        self.pos = pos
        self.pos_x = pos[0]
        self.pos_y = pos[1]
        self.tex2d = tex2d
        self.hitbox = pygame.Rect(0, 0, 128, 128)
        self.animationSpeed = 0.2
        self.currentFrame = 0
        self.animationCompleted = False

    def animator(self):
        self.currentFrame = self.currentFrame + self.animationSpeed
        clamp = max(min(self.currentFrame, 16), 0)
        self.currentFrame = clamp
        
        if self.currentFrame >= 16.0:
           self.animationCompleted = True

    def update(self):
        self.animator()
        self.hitbox = pygame.Rect(self.pos_x, self.pos_y, self.hitbox.w, self.hitbox.h)

    def draw(self, screen):
        screen.blit(self.tex2d, pygame.Rect(self.pos_x, self.pos_y, self.hitbox.w, self.hitbox.h), pygame.Rect(int(self.currentFrame) * 128, 0, self.hitbox.w, self.hitbox.h))