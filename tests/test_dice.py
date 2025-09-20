"""
掷骰子模块测试
Dice rolling module tests

This module contains comprehensive tests for the dice rolling functionality
following Test-Driven Development (TDD) principles.
"""

import pytest
from src.game.dice import Dice


class TestDice:
    """掷骰子功能测试类 / Dice functionality test class"""
    
    def test_dice_initialization(self):
        """测试骰子初始化 / Test dice initialization"""
        dice = Dice()
        assert dice is not None
        assert hasattr(dice, 'roll')
    
    def test_single_dice_roll_range(self):
        """测试单个骰子投掷范围 / Test single dice roll range"""
        dice = Dice()
        for _ in range(100):  # Test multiple times for randomness
            result = dice.roll()
            assert 1 <= result <= 6, f"Dice roll {result} is out of range"
    
    def test_single_dice_roll_type(self):
        """测试骰子返回类型 / Test dice return type"""
        dice = Dice()
        result = dice.roll()
        assert isinstance(result, int), "Dice roll should return an integer"
    
    def test_double_dice_roll_range(self):
        """测试双骰子投掷范围 / Test double dice roll range"""
        dice = Dice()
        for _ in range(100):
            result = dice.roll_double()
            assert 2 <= result <= 12, f"Double dice roll {result} is out of range"
    
    def test_double_dice_roll_type(self):
        """测试双骰子返回类型 / Test double dice return type"""
        dice = Dice()
        result = dice.roll_double()
        assert isinstance(result, int), "Double dice roll should return an integer"
    
    def test_dice_roll_randomness(self):
        """测试骰子随机性 / Test dice randomness"""
        dice = Dice()
        results = [dice.roll() for _ in range(100)]
        unique_results = set(results)
        # Should have multiple different values in 100 rolls
        assert len(unique_results) > 1, "Dice should produce random results"
        # All results should be in valid range
        assert all(1 <= result <= 6 for result in results)
    
    def test_double_dice_distribution(self):
        """测试双骰子分布 / Test double dice distribution"""
        dice = Dice()
        results = [dice.roll_double() for _ in range(1000)]
        unique_results = set(results)
        # Should have multiple different values
        assert len(unique_results) > 5, "Double dice should produce varied results"
        # All results should be in valid range
        assert all(2 <= result <= 12 for result in results)
    
    def test_dice_roll_with_seed(self):
        """测试带种子的骰子投掷 / Test dice roll with seed for reproducibility"""
        dice1 = Dice(seed=42)
        dice2 = Dice(seed=42)
        
        # Same seed should produce same sequence
        results1 = [dice1.roll() for _ in range(10)]
        results2 = [dice2.roll() for _ in range(10)]
        assert results1 == results2, "Same seed should produce same results"
    
    def test_get_last_roll(self):
        """测试获取最后一次投掷结果 / Test get last roll result"""
        dice = Dice()
        result = dice.roll()
        assert dice.get_last_roll() == result
        
        new_result = dice.roll_double()
        assert dice.get_last_roll() == new_result


class TestDiceEdgeCases:
    """骰子边缘情况测试 / Dice edge cases tests"""
    
    def test_multiple_consecutive_rolls(self):
        """测试连续多次投掷 / Test multiple consecutive rolls"""
        dice = Dice()
        for _ in range(1000):
            single = dice.roll()
            double = dice.roll_double()
            assert 1 <= single <= 6
            assert 2 <= double <= 12
    
    def test_dice_string_representation(self):
        """测试骰子字符串表示 / Test dice string representation"""
        dice = Dice()
        dice.roll()
        str_repr = str(dice)
        assert isinstance(str_repr, str)
        assert len(str_repr) > 0