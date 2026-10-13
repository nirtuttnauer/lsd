"""Dependency-free CLI regression tests; pass a compiled lsd executable."""
import pathlib
import subprocess
import sys
import tempfile
import unittest

BINARY = pathlib.Path(sys.argv.pop(1)).resolve()


class DirectoryArguments(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = pathlib.Path(self.temp.name)
        (self.root / "child").mkdir()
        (self.root / "child" / "visible.txt").write_text("hello")
        (self.root / "child" / ".hidden.txt").write_text("hidden")

    def run_cli(self, *args):
        return subprocess.run([str(BINARY), *args], cwd=self.root,
                              capture_output=True, text=True, timeout=5)

    def test_default_directory(self):
        result = self.run_cli()
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("child", result.stdout)
        self.assertIn("\tvisible.txt", result.stdout)
        self.assertNotIn(".hidden.txt", result.stdout)

    def test_relative_directory(self):
        result = self.run_cli("child")
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("\nvisible.txt\n", result.stdout)
        self.assertNotIn("\tvisible.txt", result.stdout)

    def test_absolute_directory_with_spaces(self):
        target = self.root / "directory with spaces"
        target.mkdir()
        (target / "example.txt").write_text("example")
        result = self.run_cli(str(target))
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("example.txt", result.stdout)

    def test_options_before_and_after_directory(self):
        for args in [("-", "child", "+"), ("child", "-", "+")]:
            with self.subTest(args=args):
                result = self.run_cli(*args)
                self.assertEqual(result.returncode, 0, result.stderr)
                self.assertIn(".hidden.txt", result.stdout)
                self.assertIn("Text File", result.stdout)

    def test_second_directory_rejected(self):
        result = self.run_cli("child", "other")
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("Unknown argument: other", result.stderr)


if __name__ == "__main__":
    unittest.main()
