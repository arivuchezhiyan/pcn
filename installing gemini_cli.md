# Installing Gemini CLI

## Step 1: Create Folder, Virtual Environment & Install Package
```bash
# Ensure python3-venv is installed on Ubuntu
sudo apt update && sudo apt install python3-venv python3-pip -y

# Create and enter directory in home folder
mkdir -p ~/openai-terminal
cd ~/openai-terminal

# Create the Python virtual environment (.venv)
python3 -m venv .venv

# Activate the virtual environment
source .venv/bin/activate

# Upgrade pip and install the Google GenAI SDK
python3 -m pip install --upgrade google-genai
```

## Step 2: Set Gemini API Key
```bash
read -rsp "Paste your Gemini API key: " GEMINI_API_KEY
echo
export GEMINI_API_KEY

# Optional: Persist for future terminal sessions so you don't have to re-enter it
# echo "export GEMINI_API_KEY='$GEMINI_API_KEY'" >> ~/.bashrc
```

## Step 3: Test the API Connection
```bash
python3 -c 'from google import genai; c=genai.Client(); r=c.models.generate_content(model="gemini-2.5-flash", contents="Say hello bro"); print(r.text)'
```

## Step 4: Create the Interactive Chat Script
```bash
nano gemini_chat.py
```

Paste the following code into `gemini_chat.py`:
```python
from google import genai

client = genai.Client()
chat = client.chats.create(model="gemini-2.5-flash")

print("\n🤖 Gemini Terminal Chat")
print("Type 'exit' to quit or 'reset' to restart.\n")

while True:
    try:
        prompt = input("You: ").strip()

        if not prompt:
            continue

        if prompt.lower() in ("exit", "quit"):
            print("Gemini: See you, bro!")
            break

        if prompt.lower() == "reset":
            chat = client.chats.create(model="gemini-2.5-flash")
            print("Gemini: Conversation reset!\n")
            continue

        response = chat.send_message(prompt)
        print(f"\nGemini: {response.text}\n")

    except KeyboardInterrupt:
        print("\nChat ended.")
        break
    except Exception as error:
        print(f"\nError: {error}\n")
```

## Final Step: Launch the Chat
```bash
python3 gemini_chat.py
```
