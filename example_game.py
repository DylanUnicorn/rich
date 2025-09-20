"""
游戏示例 / Game Example

This module demonstrates how to use the dice functionality 
in a simple game context.
"""

from src.game.dice import Dice
import colorama
from colorama import Fore, Style

colorama.init()


class SimpleGame:
    """
    简单游戏示例 / Simple game example
    
    Demonstrates the usage of dice in a basic game context.
    """
    
    def __init__(self):
        """初始化游戏 / Initialize game"""
        self.dice = Dice()
        self.player_position = 0
        self.game_board_size = 40  # Standard Monopoly board size
    
    def play_turn(self) -> dict:
        """
        玩一回合 / Play a turn
        
        Returns:
            dict: 回合结果信息 / Turn result information
        """
        print(f"{Fore.YELLOW}🎲 准备投掷骰子... / Preparing to roll dice...{Style.RESET_ALL}")
        
        # Roll double dice for movement
        roll_result = self.dice.roll_double()
        
        # Update player position
        old_position = self.player_position
        self.player_position = (self.player_position + roll_result) % self.game_board_size
        
        # Check if player passed GO (position 0)
        passed_go = old_position + roll_result >= self.game_board_size
        
        turn_info = {
            'roll': roll_result,
            'old_position': old_position,
            'new_position': self.player_position,
            'passed_go': passed_go
        }
        
        self._display_turn_result(turn_info)
        return turn_info
    
    def _display_turn_result(self, turn_info: dict):
        """
        显示回合结果 / Display turn result
        
        Args:
            turn_info: 回合信息 / Turn information
        """
        print(f"{Fore.GREEN}🎯 投掷结果: {turn_info['roll']} / Roll result: {turn_info['roll']}{Style.RESET_ALL}")
        print(f"{Fore.BLUE}📍 从位置 {turn_info['old_position']} 移动到 {turn_info['new_position']} / Moved from position {turn_info['old_position']} to {turn_info['new_position']}{Style.RESET_ALL}")
        
        if turn_info['passed_go']:
            print(f"{Fore.MAGENTA}💰 经过起点！获得奖励！/ Passed GO! Collect reward!{Style.RESET_ALL}")
    
    def play_demo(self, turns: int = 5):
        """
        运行演示游戏 / Run demo game
        
        Args:
            turns: 游戏回合数 / Number of turns to play
        """
        print(f"{Fore.CYAN}{'='*50}")
        print(f"🎮 大富翁游戏演示 / Monopoly Game Demo")
        print(f"{'='*50}{Style.RESET_ALL}")
        
        for turn_num in range(1, turns + 1):
            print(f"\n{Fore.WHITE}--- 第 {turn_num} 回合 / Turn {turn_num} ---{Style.RESET_ALL}")
            self.play_turn()
        
        print(f"\n{Fore.CYAN}🏁 游戏演示结束 / Demo game finished!")
        print(f"最终位置 / Final position: {self.player_position}{Style.RESET_ALL}")


def main():
    """主函数 / Main function"""
    try:
        # Create and run demo game
        game = SimpleGame()
        game.play_demo(5)
        
        print(f"\n{Fore.GREEN}✨ 感谢试玩！/ Thanks for playing!{Style.RESET_ALL}")
        
    except KeyboardInterrupt:
        print(f"\n{Fore.YELLOW}🛑 游戏被用户中断 / Game interrupted by user{Style.RESET_ALL}")
    except Exception as e:
        print(f"\n{Fore.RED}❌ 游戏错误: {e} / Game error: {e}{Style.RESET_ALL}")


if __name__ == "__main__":
    main()