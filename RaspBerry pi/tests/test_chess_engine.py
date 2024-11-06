import unittest
from src.chess_engine import ChessEngine

class TestChessEngine(unittest.TestCase):
    def setUp(self):
        self.engine = ChessEngine()
        self.engine.start_engine()

    def tearDown(self):
        self.engine.stop_engine()

    def test_engine_initialization(self):
        self.assertIsNotNone(self.engine.engine)
        self.assertTrue(self.engine.board.is_game_over() == False)

    def test_get_best_move(self):
        best_move = self.engine.get_best_move()
        self.assertIsNotNone(best_move)
        self.assertIn(best_move, self.engine.board.legal_moves)

if __name__ == '__main__':
    unittest.main()
