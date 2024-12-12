import os
import re

# Define the regex pattern for Chinese characters
chinese_pattern = re.compile(r'[\u4e00-\u9fff]')

for root, dirs, files in os.walk('.'):
    for file in files:
        filepath = os.path.join(root, file)
        try:
            with open(filepath, 'r', encoding='utf-8') as f:
                for i, line in enumerate(f, start=1):
                    match = chinese_pattern.search(line)
                    if match:
                        print(f"{filepath}:{i}: {line.strip()}")
                        print(f"Found Chinese character(s): {match.group()}")
        except Exception as e:
            print(f"Could not read {filepath}: {e}")
