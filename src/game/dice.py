"""
掷骰子模块
Dice rolling module

This module provides dice rolling functionality for the Monopoly game.
"""

import random
from typing import Optional


class Dice:
    """
    骰子类 / Dice class
    
    Provides functionality for rolling single and double dice.
    Supports seeding for reproducible results in testing.
    """
    
    def __init__(self, seed: Optional[int] = None):
        """
        初始化骰子 / Initialize dice
        
        Args:
            seed: 随机种子，用于测试时的可重现性 / Random seed for reproducibility in tests
        """
        self._random = random.Random(seed)
        self._last_roll: Optional[int] = None
    
    def roll(self) -> int:
        """
        投掷单个骰子 / Roll a single dice
        
        Returns:
            int: 1-6之间的随机数 / Random number between 1-6
        """
        result = self._random.randint(1, 6)
        self._last_roll = result
        return result
    
    def roll_double(self) -> int:
        """
        投掷两个骰子 / Roll two dice
        
        Returns:
            int: 2-12之间的随机数 / Random number between 2-12
        """
        dice1 = self._random.randint(1, 6)
        dice2 = self._random.randint(1, 6)
        result = dice1 + dice2
        self._last_roll = result
        return result
    
    def get_last_roll(self) -> Optional[int]:
        """
        获取最后一次投掷结果 / Get last roll result
        
        Returns:
            Optional[int]: 最后一次投掷的结果，如果没有投掷过则返回None / 
                          Last roll result, None if no roll has been made
        """
        return self._last_roll
    
    def __str__(self) -> str:
        """
        字符串表示 / String representation
        
        Returns:
            str: 骰子的字符串描述 / String description of the dice
        """
        if self._last_roll is not None:
            return f"Dice(last_roll={self._last_roll})"
        else:
            return "Dice(no_rolls_yet)"