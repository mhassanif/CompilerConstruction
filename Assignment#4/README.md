# json2relcsv

`json2relcsv` is a command-line tool that converts JSON input into a relational schema and outputs it as CSV files or to the terminal. It parses JSON using Flex and Bison, builds an Abstract Syntax Tree (AST), and transforms it into relational tables, handling nested objects and arrays with foreign keys and junction tables.

## Features
- Converts JSON to relational CSV files.
- Supports nested objects (child tables) and arrays (junction tables).
- Outputs CSV to files or terminal (`--print-csv`).
- Prints AST for debugging (`--print-ast`).
- Handles JSON literals (strings, numbers, booleans, null).

## Prerequisites
- **gcc**: C compiler.
- **flex**: Lexical analyzer generator.
- **bison**: Parser generator.
- **make**: Build automation tool.
- A Unix-like system (e.g., Linux, macOS).

## Installation
1. Clone or download the repository.
2. Navigate to the project directory:
   ```bash
   cd json2relcsv
   ```
3. Build the project:
   ```bash
   make
   ```
   This generates the `json2relcsv` executable in the `src` directory.

## Usage
Run the program with JSON input via stdin and optional flags:
```bash
./src/json2relcsv [options] < input.json
```
**Options**:
- `--print-ast`: Prints the AST to stdout.
- `--print-csv`: Prints CSV data to the terminal instead of files.
- `--out-dir <dir>`: Specifies the output directory for CSV files (default: `.`).

### Running Tests
The project includes a test script (`test.sh`) to run the program with a test JSON file. The script is hardcoded to use `tests/Test1.json`. To run it with a different JSON file, you must edit `test.sh`.

#### Steps to Run a Test
1. **Edit `test.sh`**:
   Open `test.sh` and replace `Test1.json` with your JSON file name (e.g., `MyTest.json`):
   ```bash
   ./src/json2relcsv < ./tests/MyTest.json > output.csv
   ./src/json2relcsv --print-csv < ./tests/MyTest.json
   ```
2. **Ensure the JSON file exists**:
   Place your JSON file (e.g., `MyTest.json`) in the `tests` directory.
3. **Run the test**:
   ```bash
   sh test.sh
   ```
   This will:
   - Generate `output.csv` in the current directory (from the first command).
   - Print CSV data to the terminal (from the second command).

#### Example
Create a file `tests/MyTest.json` with:
```json
{
    "movie": "Inception",
    "genres": ["Action", "Sci-Fi", "Thriller"]
}
```
Edit `test.sh` to use `MyTest.json`:
```bash
./src/json2relcsv < ./tests/MyTest.json > output.csv
./src/json2relcsv --print-csv < ./tests/MyTest.json
```
Run:
```bash
sh test.sh
```
**Terminal Output** (from `--print-csv`):
```
Table: root_genres
id,parent_id,index,value
2,1,0,Action
3,1,1,Sci-Fi
4,1,2,Thriller

Table: root
id,movie,genres
1,Inception,
```
**File Output**:
- `output.csv` contains the `root` table (same as above, depending on implementation).
- Additional CSV files (`root.csv`, `root_genres.csv`) may be created in the output directory if `--out-dir` is used.

## Project Structure
- **src/**: Source files.
  - `ast.c`, `ast.h`: AST creation and management.
  - `schema.c`, `schema.h`: Relational schema generation and CSV output.
  - `scanner.l`: Lexical analyzer (Flex).
  - `parser.y`: Parser (Bison).
  - `main.c`: Program entry point.
- **tests/**: Test JSON files (e.g., `Test1.json`).
- **test.sh**: Test script.
- **Makefile**: Build instructions.

## Error Handling
- **Lexical Errors**: Invalid characters (e.g., `@`) print an error with line/column and exit.
- **Syntax Errors**: Invalid JSON (e.g., missing comma) prints an error and exits.
- **File I/O Errors**: Failure to open output files prints an error and exits.
- **Memory Errors**: Allocation failures silently skip affected nodes (no user notification).

## Limitations
- Hardcoded test file in `test.sh` requires manual editing.
- No error recovery; errors cause program termination.
- Limited type handling (numbers stored as strings).

## Future Improvements
- Parameterize `test.sh` to accept JSON file names as arguments.
- Add error recovery for robust parsing.
- Support additional output formats (e.g., SQL).

## License
This project is unlicensed. Use and modify at your own risk.