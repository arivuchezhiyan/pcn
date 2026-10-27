#!/usr/bin/env python3
"""
=============================================================================
 GEMINI UI PRO - Modern Web Dashboard & Terminal Agent
=============================================================================
Features:
- Sleek Web UI (Auto-opens in browser, zero extra web framework dependencies)
- Dynamic Model Switching (gemini-2.5-flash, gemini-2.5-pro, etc.)
- Multi-API Key Management & Automatic Failover Rotation from 'gemini' file
- Real-time Token Streaming (Server-Sent Events)
- Markdown & Code Block Syntax Highlighting with 1-click Copy
- Google Search Grounding toggle
- System Persona Presets
- Terminal CLI Mode fallback (via --cli flag)
=============================================================================
"""

import os
import re
import sys
import json
import socket
import threading
import webbrowser
from http.server import HTTPServer, BaseHTTPRequestHandler
from urllib.parse import urlparse

# Try importing Google GenAI SDK
try:
    from google import genai
    from google.genai import types
    HAS_GENAI = True
except ImportError:
    HAS_GENAI = False

DEFAULT_MODELS = [
    {"id": "gemini-3.5-flash-lite", "name": "Gemini 3.5 Flash Lite", "badge": "Fastest & Ultra Reliable"},
    {"id": "gemini-3.8-flash", "name": "Gemini 3.8 Flash", "badge": "High Demand"},
    {"id": "gemini-3.8-pro", "name": "Gemini 3.8 Pro", "badge": "Deep Reasoning"}
]

SYSTEM_PERSONAS = {
    "developer": "You are an elite senior software engineer. Provide high-quality, production-ready code with concise, clear explanations.",
    "general": "You are an intelligent, helpful, and concise AI assistant.",
    "security": "You are a cybersecurity expert and ethical hacker. Focus on vulnerability analysis, secure coding, and defense mechanisms.",
    "writer": "You are a professional technical writer and communicator. Explain concepts with clarity, structure, and engaging tone."
}

def load_keys():
    """Parse API keys from the local 'gemini' file or fallback to environment."""
    keys = {}
    base_dir = os.path.dirname(os.path.abspath(__file__))
    gemini_path = os.path.join(base_dir, "gemini")
    
    if os.path.exists(gemini_path):
        try:
            with open(gemini_path, "r", encoding="utf-8") as f:
                content = f.read()
            
            # Find labelled pairs like API1 \n <key>
            pattern = re.findall(r"(API\d+)\s*\n\s*([A-Za-z0-9_\-\.]+)", content, re.IGNORECASE)
            for label, key in pattern:
                keys[label.upper()] = key.strip()
                
            # If regex didn't catch pairs, grab any tokens
            if not keys:
                tokens = re.findall(r"arivu[A-Za-z0-9_\-\.]+arivu", content)
                for i, token in enumerate(tokens, 1):
                    keys[f"API{i}"] = token.strip()
        except Exception as e:
            print(f"[Warning] Error reading gemini file: {e}")

    # Fallback if no keys found
    if not keys:
        env_key = os.environ.get("GEMINI_API_KEY", "").strip()
        if env_key:
            keys["ENV_KEY"] = env_key
        else:
            keys["API1"] = "demo-key-replace-in-gemini-file"

    return keys

class GeminiManager:
    """Manages Gemini client sessions, model switching, and key rotation."""
    def __init__(self):
        self.keys = load_keys()
        self.key_list = list(self.keys.keys())
        self.current_key_id = self.key_list[0] if self.key_list else "API1"
        self.model = "gemini-3.5-flash-lite"
        self.system_prompt = SYSTEM_PERSONAS["developer"]
        self.search_grounding = False
        self.auto_rotate = True
        self.chat_session = None
        self.client = None
        self.init_client()

    def get_api_key(self, key_id=None):
        kid = key_id or self.current_key_id
        return self.keys.get(kid, "")

    def init_client(self):
        if not HAS_GENAI:
            return
        key = self.get_api_key()
        os.environ["GEMINI_API_KEY"] = key
        self.client = genai.Client(api_key=key)
        
        tools = [{"google_search": {}}] if self.search_grounding else None
        config = types.GenerateContentConfig(
            system_instruction=self.system_prompt,
            tools=tools
        )
        self.chat_session = self.client.chats.create(model=self.model, config=config)

    def rotate_key(self):
        if len(self.key_list) <= 1:
            return self.current_key_id
        idx = (self.key_list.index(self.current_key_id) + 1) % len(self.key_list)
        self.current_key_id = self.key_list[idx]
        self.init_client()
        return self.current_key_id

    def set_key(self, key_id):
        if key_id in self.keys:
            self.current_key_id = key_id
            self.init_client()
            return True
        return False

    def set_model(self, model_id):
        self.model = model_id
        self.init_client()

    def set_persona(self, persona_text):
        self.system_prompt = persona_text
        self.init_client()

    def set_search(self, enabled):
        self.search_grounding = bool(enabled)
        self.init_client()

manager = GeminiManager()

HTML_PAGE = """<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Gemini Pro Studio</title>
  <link rel="preconnect" href="https://fonts.googleapis.com">
  <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
  <link href="https://fonts.googleapis.com/css2?family=Plus+Jakarta+Sans:wght@400;500;600;700&family=JetBrains+Mono:wght@400;500&display=swap" rel="stylesheet">
  <style>
    :root {
      --bg-base: #0a0d14;
      --bg-surface: #111724;
      --bg-panel: #161e30;
      --bg-card: rgba(26, 36, 56, 0.7);
      --border: rgba(255, 255, 255, 0.08);
      --border-focus: #6366f1;
      --primary: #4f46e5;
      --primary-hover: #4338ca;
      --primary-glow: rgba(99, 102, 241, 0.25);
      --accent: #06b6d4;
      --accent-glow: rgba(6, 182, 212, 0.2);
      --emerald: #10b981;
      --emerald-glow: rgba(16, 185, 129, 0.2);
      --amber: #f59e0b;
      --text: #f1f5f9;
      --text-muted: #94a3b8;
      --radius-sm: 8px;
      --radius-md: 14px;
      --radius-lg: 20px;
      --transition: all 0.2s cubic-bezier(0.4, 0, 0.2, 1);
    }

    * { box-sizing: border-box; margin: 0; padding: 0; }
    body {
      font-family: 'Plus Jakarta Sans', -apple-system, sans-serif;
      background: radial-gradient(circle at top right, #131b2e, var(--bg-base) 60%);
      color: var(--text);
      height: 100vh;
      display: flex;
      overflow: hidden;
    }

    /* Sidebar */
    .sidebar {
      width: 310px;
      background: var(--bg-surface);
      border-right: 1px solid var(--border);
      display: flex;
      flex-direction: column;
      padding: 20px;
      gap: 20px;
      overflow-y: auto;
      z-index: 10;
    }

    .brand {
      display: flex;
      align-items: center;
      gap: 12px;
      padding-bottom: 15px;
      border-bottom: 1px solid var(--border);
    }
    .brand-icon {
      width: 40px;
      height: 40px;
      border-radius: var(--radius-sm);
      background: linear-gradient(135deg, #6366f1, #06b6d4);
      display: flex;
      align-items: center;
      justify-content: center;
      font-size: 20px;
      box-shadow: 0 0 15px var(--primary-glow);
    }
    .brand-title {
      font-size: 17px;
      font-weight: 700;
      letter-spacing: -0.3px;
    }
    .brand-badge {
      font-size: 10px;
      background: rgba(99, 102, 241, 0.2);
      color: #a5b4fc;
      padding: 2px 8px;
      border-radius: 20px;
      margin-left: auto;
      font-weight: 600;
      border: 1px solid rgba(99, 102, 241, 0.4);
    }

    .control-group {
      display: flex;
      flex-direction: column;
      gap: 8px;
    }
    .control-label {
      font-size: 12px;
      font-weight: 600;
      color: var(--text-muted);
      text-transform: uppercase;
      letter-spacing: 0.5px;
    }

    select, textarea {
      width: 100%;
      background: var(--bg-panel);
      border: 1px solid var(--border);
      border-radius: var(--radius-sm);
      color: var(--text);
      padding: 10px 12px;
      font-size: 13px;
      font-family: inherit;
      outline: none;
      transition: var(--transition);
    }
    select:focus, textarea:focus {
      border-color: var(--border-focus);
      box-shadow: 0 0 0 2px var(--primary-glow);
    }

    .key-grid {
      display: grid;
      grid-template-columns: repeat(3, 1fr);
      gap: 6px;
    }
    .key-pill {
      background: var(--bg-panel);
      border: 1px solid var(--border);
      border-radius: 6px;
      padding: 7px 4px;
      font-size: 11px;
      font-weight: 600;
      color: var(--text-muted);
      cursor: pointer;
      text-align: center;
      transition: var(--transition);
    }
    .key-pill:hover {
      border-color: var(--border-focus);
      color: var(--text);
    }
    .key-pill.active {
      background: rgba(16, 185, 129, 0.15);
      border-color: var(--emerald);
      color: #34d399;
      box-shadow: 0 0 10px var(--emerald-glow);
    }

    .toggle-row {
      display: flex;
      align-items: center;
      justify-content: space-between;
      padding: 10px 12px;
      background: var(--bg-panel);
      border: 1px solid var(--border);
      border-radius: var(--radius-sm);
      font-size: 13px;
    }
    .switch {
      position: relative;
      display: inline-block;
      width: 36px;
      height: 20px;
    }
    .switch input { opacity: 0; width: 0; height: 0; }
    .slider {
      position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0;
      background-color: #334155;
      transition: .3s;
      border-radius: 20px;
    }
    .slider:before {
      position: absolute; content: ""; height: 14px; width: 14px; left: 3px; bottom: 3px;
      background-color: white;
      transition: .3s;
      border-radius: 50%;
    }
    input:checked + .slider { background-color: var(--primary); }
    input:checked + .slider:before { transform: translateX(16px); }

    .btn-secondary {
      background: var(--bg-panel);
      border: 1px solid var(--border);
      color: var(--text-muted);
      padding: 10px;
      border-radius: var(--radius-sm);
      font-weight: 600;
      font-size: 13px;
      cursor: pointer;
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 8px;
      transition: var(--transition);
    }
    .btn-secondary:hover {
      background: rgba(255, 255, 255, 0.05);
      color: var(--text);
      border-color: rgba(255, 255, 255, 0.2);
    }

    /* Main Chat */
    .main-content {
      flex: 1;
      display: flex;
      flex-direction: column;
      height: 100vh;
      position: relative;
    }

    .header {
      height: 65px;
      border-bottom: 1px solid var(--border);
      background: rgba(17, 23, 36, 0.8);
      backdrop-filter: blur(12px);
      display: flex;
      align-items: center;
      justify-content: space-between;
      padding: 0 28px;
    }
    .status-badge {
      display: flex;
      align-items: center;
      gap: 8px;
      font-size: 13px;
      color: var(--text-muted);
    }
    .status-dot {
      width: 8px;
      height: 8px;
      background: var(--emerald);
      border-radius: 50%;
      box-shadow: 0 0 8px var(--emerald);
    }

    .messages-container {
      flex: 1;
      overflow-y: auto;
      padding: 28px;
      display: flex;
      flex-direction: column;
      gap: 20px;
      scroll-behavior: smooth;
    }

    .welcome-hero {
      margin: auto;
      max-width: 600px;
      text-align: center;
      display: flex;
      flex-direction: column;
      gap: 16px;
      padding: 30px;
      background: var(--bg-card);
      border: 1px solid var(--border);
      border-radius: var(--radius-lg);
      backdrop-filter: blur(10px);
    }
    .welcome-hero h2 {
      font-size: 24px;
      font-weight: 700;
      background: linear-gradient(135deg, #fff, #94a3b8);
      -webkit-background-clip: text;
      -webkit-text-fill-color: transparent;
    }
    .welcome-hero p {
      color: var(--text-muted);
      font-size: 14px;
      line-height: 1.6;
    }
    .suggestions {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 10px;
      margin-top: 10px;
    }
    .suggestion-card {
      background: var(--bg-panel);
      border: 1px solid var(--border);
      padding: 12px 14px;
      border-radius: var(--radius-sm);
      font-size: 12px;
      text-align: left;
      cursor: pointer;
      color: var(--text);
      transition: var(--transition);
    }
    .suggestion-card:hover {
      border-color: var(--border-focus);
      background: rgba(99, 102, 241, 0.1);
      transform: translateY(-2px);
    }

    .message-row {
      display: flex;
      gap: 14px;
      max-width: 850px;
      width: 100%;
      margin: 0 auto;
    }
    .message-row.user {
      justify-content: flex-end;
    }
    .avatar {
      width: 34px;
      height: 34px;
      border-radius: var(--radius-sm);
      display: flex;
      align-items: center;
      justify-content: center;
      font-size: 15px;
      flex-shrink: 0;
    }
    .avatar.agent {
      background: linear-gradient(135deg, #6366f1, #06b6d4);
      color: white;
    }
    .avatar.user {
      background: #3b82f6;
      color: white;
    }

    .bubble {
      padding: 14px 18px;
      border-radius: var(--radius-md);
      font-size: 14px;
      line-height: 1.6;
      max-width: 85%;
      word-break: break-word;
    }
    .user .bubble {
      background: #312e81;
      color: #e0e7ff;
      border-bottom-right-radius: 4px;
      border: 1px solid rgba(99, 102, 241, 0.3);
    }
    .agent .bubble {
      background: var(--bg-surface);
      border: 1px solid var(--border);
      color: var(--text);
      border-bottom-left-radius: 4px;
      box-shadow: 0 4px 20px rgba(0, 0, 0, 0.2);
    }

    /* Code blocks */
    pre {
      background: #080b11;
      border: 1px solid rgba(255, 255, 255, 0.08);
      border-radius: 8px;
      padding: 12px;
      margin: 12px 0;
      overflow-x: auto;
      font-family: 'JetBrains Mono', monospace;
      font-size: 13px;
      position: relative;
    }
    code {
      font-family: 'JetBrains Mono', monospace;
      color: #38bdf8;
      background: rgba(56, 189, 248, 0.1);
      padding: 2px 5px;
      border-radius: 4px;
      font-size: 12px;
    }
    pre code {
      color: #e2e8f0;
      background: transparent;
      padding: 0;
    }
    .copy-btn {
      position: absolute;
      top: 8px;
      right: 8px;
      background: #1e293b;
      border: 1px solid var(--border);
      color: var(--text-muted);
      border-radius: 4px;
      padding: 4px 8px;
      font-size: 11px;
      cursor: pointer;
      transition: var(--transition);
    }
    .copy-btn:hover { color: white; background: #334155; }

    /* Input area */
    .input-container {
      padding: 16px 28px 24px;
      background: rgba(10, 13, 20, 0.95);
      border-top: 1px solid var(--border);
      max-width: 900px;
      width: 100%;
      margin: 0 auto;
    }
    .input-box {
      background: var(--bg-surface);
      border: 1px solid var(--border);
      border-radius: var(--radius-md);
      padding: 8px 12px;
      display: flex;
      align-items: flex-end;
      gap: 10px;
      transition: var(--transition);
      box-shadow: 0 4px 20px rgba(0, 0, 0, 0.3);
    }
    .input-box:focus-within {
      border-color: var(--border-focus);
      box-shadow: 0 0 0 2px var(--primary-glow);
    }
    textarea#user-prompt {
      flex: 1;
      background: transparent;
      border: none;
      color: var(--text);
      font-size: 14px;
      resize: none;
      padding: 6px 0;
      max-height: 140px;
      line-height: 1.5;
    }
    textarea#user-prompt:focus { box-shadow: none; border: none; }
    .send-btn {
      background: var(--primary);
      border: none;
      color: white;
      width: 36px;
      height: 36px;
      border-radius: var(--radius-sm);
      display: flex;
      align-items: center;
      justify-content: center;
      cursor: pointer;
      transition: var(--transition);
      flex-shrink: 0;
    }
    .send-btn:hover {
      background: var(--primary-hover);
      transform: scale(1.05);
      box-shadow: 0 0 12px var(--primary-glow);
    }
    .send-btn:disabled {
      background: #334155;
      cursor: not-allowed;
      transform: none;
    }
    .input-footer {
      display: flex;
      justify-content: space-between;
      font-size: 11px;
      color: var(--text-muted);
      margin-top: 8px;
      padding: 0 4px;
    }
  </style>
</head>
<body>

  <!-- Sidebar -->
  <aside class="sidebar">
    <div class="brand">
      <div class="brand-icon">⚡</div>
      <div>
        <div class="brand-title">Gemini UI Pro</div>
        <div style="font-size: 11px; color: var(--text-muted);">Autonomous Studio</div>
      </div>
      <div class="brand-badge">PRO</div>
    </div>

    <!-- Model Selector -->
    <div class="control-group">
      <label class="control-label">Gemini Model</label>
      <select id="model-select" onchange="updateModel(this.value)">
        <!-- Injected via JS -->
      </select>
    </div>

    <!-- API Key Matrix -->
    <div class="control-group">
      <div style="display: flex; justify-content: space-between; align-items: center;">
        <label class="control-label">API Keys (Active / Quota)</label>
        <span id="active-key-indicator" style="font-size: 11px; color: #34d399; font-weight: 600;">API1</span>
      </div>
      <div class="key-grid" id="key-matrix">
        <!-- Injected via JS -->
      </div>
    </div>

    <!-- Auto-Rotate Quota Switch -->
    <div class="toggle-row">
      <span>Auto-Rotate on Limit (429)</span>
      <label class="switch">
        <input type="checkbox" id="auto-rotate-switch" checked>
        <span class="slider"></span>
      </label>
    </div>

    <!-- Google Search Grounding -->
    <div class="toggle-row">
      <span>Google Search Live</span>
      <label class="switch">
        <input type="checkbox" id="search-switch" onchange="toggleSearch(this.checked)">
        <span class="slider"></span>
      </label>
    </div>

    <!-- Persona Selector -->
    <div class="control-group">
      <label class="control-label">Agent Persona</label>
      <select id="persona-select" onchange="updatePersona(this.value)">
        <option value="developer">💻 Senior Software Architect</option>
        <option value="general">🤖 General AI Assistant</option>
        <option value="security">🛡️ Cybersecurity Specialist</option>
        <option value="writer">📝 Technical Communicator</option>
      </select>
    </div>

    <div style="margin-top: auto; display: flex; flex-direction: column; gap: 8px;">
      <button class="btn-secondary" onclick="resetChat()">
        🔄 Reset Conversation
      </button>
      <button class="btn-secondary" onclick="exportChat()">
        📥 Export Markdown
      </button>
    </div>
  </aside>

  <!-- Main Chat Section -->
  <main class="main-content">
    <header class="header">
      <div class="status-badge">
        <span class="status-dot"></span>
        <span id="header-status">Online • Ready • gemini-2.5-flash</span>
      </div>
      <div style="font-size: 12px; color: var(--text-muted);">
        Connected to local Python server
      </div>
    </header>

    <div class="messages-container" id="chat-messages">
      <div class="welcome-hero" id="hero-banner">
        <div style="font-size: 36px;">🤖</div>
        <h2>Welcome to Gemini UI Pro</h2>
        <p>Your interactive terminal agent upgraded with full web capabilities, multi-key auto failover, and model switching.</p>
        <div class="suggestions">
          <div class="suggestion-card" onclick="sendPrompt('Write a clean Python script to test network connectivity and log latencies.')">
            🌐 Network Latency Script
          </div>
          <div class="suggestion-card" onclick="sendPrompt('Explain the differences between Gemini 2.5 Flash and Pro models.')">
            ⚡ Gemini Flash vs Pro
          </div>
          <div class="suggestion-card" onclick="sendPrompt('Show a modern bash script template with error handling and traps.')">
            💻 Robust Bash Script
          </div>
          <div class="suggestion-card" onclick="sendPrompt('How can I optimize Python code for high-throughput streaming?')">
            🚀 High-Throughput Python
          </div>
        </div>
      </div>
    </div>

    <div class="input-container">
      <div class="input-box">
        <textarea id="user-prompt" rows="1" placeholder="Type a message or instruction... (Shift + Enter for new line)" onkeydown="handleKeyDown(event)"></textarea>
        <button class="send-btn" id="send-button" onclick="sendMessage()">
          ➤
        </button>
      </div>
      <div class="input-footer">
        <span>Powered by Google GenAI SDK</span>
        <span>Enter to send • Shift+Enter for newline</span>
      </div>
    </div>
  </main>

  <script>
    let appConfig = {};
    let isGenerating = false;

    async function loadConfig() {
      try {
        const res = await fetch('/api/config');
        appConfig = await res.json();
        
        // Models dropdown
        const modelSelect = document.getElementById('model-select');
        modelSelect.innerHTML = appConfig.models.map(m => 
          `<option value="${m.id}" ${m.id === appConfig.current_model ? 'selected' : ''}>${m.name} (${m.badge})</option>`
        ).join('');

        // Keys matrix
        renderKeyMatrix();
        updateStatus();
      } catch (e) {
        console.error("Failed to load config", e);
      }
    }

    function renderKeyMatrix() {
      const matrix = document.getElementById('key-matrix');
      matrix.innerHTML = Object.keys(appConfig.keys).map(k => `
        <div class="key-pill ${k === appConfig.current_key ? 'active' : ''}" onclick="switchKey('${k}')">
          ${k}
        </div>
      `).join('');
      document.getElementById('active-key-indicator').textContent = appConfig.current_key;
    }

    function updateStatus() {
      const searchStatus = document.getElementById('search-switch').checked ? ' • Search ON' : '';
      document.getElementById('header-status').textContent = 
        `Active: ${appConfig.current_key} • ${appConfig.current_model}${searchStatus}`;
    }

    async function switchKey(keyId) {
      const res = await fetch('/api/set_key', {
        method: 'POST',
        headers: {'Content-Type': 'application/json'},
        body: JSON.stringify({ key_id: keyId })
      });
      if (res.ok) {
        appConfig.current_key = keyId;
        renderKeyMatrix();
        updateStatus();
      }
    }

    async function updateModel(modelId) {
      await fetch('/api/set_model', {
        method: 'POST',
        headers: {'Content-Type': 'application/json'},
        body: JSON.stringify({ model: modelId })
      });
      appConfig.current_model = modelId;
      updateStatus();
    }

    async function toggleSearch(enabled) {
      await fetch('/api/set_search', {
        method: 'POST',
        headers: {'Content-Type': 'application/json'},
        body: JSON.stringify({ search: enabled })
      });
      updateStatus();
    }

    async function updatePersona(personaKey) {
      await fetch('/api/set_persona', {
        method: 'POST',
        headers: {'Content-Type': 'application/json'},
        body: JSON.stringify({ persona_key: personaKey })
      });
    }

    function handleKeyDown(e) {
      if (e.key === 'Enter' && !e.shiftKey) {
        e.preventDefault();
        sendMessage();
      }
    }

    function sendPrompt(text) {
      document.getElementById('user-prompt').value = text;
      sendMessage();
    }

    function formatMarkdown(text) {
      // Basic markdown parser for bold, code blocks, and lists
      let html = text
        .replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;")
        .replace(/```([a-zA-Z0-9]*)\\n([\\s\\S]*?)```/g, (match, lang, code) => {
          return `<pre><button class="copy-btn" onclick="navigator.clipboard.writeText(this.nextElementSibling.innerText); this.innerText='Copied!'; setTimeout(()=>this.innerText='Copy', 1500)">Copy</button><code>${code.trim()}</code></pre>`;
        })
        .replace(/`([^`]+)`/g, '<code>$1</code>')
        .replace(/\\*\\*(.*?)\\*\\*/g, '<strong>$1</strong>')
        .replace(/^\\* (.*?)$/gm, '• $1')
        .replace(/\\n/g, '<br>');
      return html;
    }

    async function sendMessage() {
      const input = document.getElementById('user-prompt');
      const text = input.value.trim();
      if (!text || isGenerating) return;

      // Hide welcome hero
      const hero = document.getElementById('hero-banner');
      if (hero) hero.style.display = 'none';

      // Append User message
      appendMessage('user', text);
      input.value = '';
      input.style.height = 'auto';

      isGenerating = true;
      document.getElementById('send-button').disabled = true;

      // Create Agent message placeholder
      const bubble = appendMessage('agent', '');
      let accumulatedText = "";

      try {
        const response = await fetch('/api/chat_stream', {
          method: 'POST',
          headers: {'Content-Type': 'application/json'},
          body: JSON.stringify({ message: text })
        });

        const reader = response.body.getReader();
        const decoder = new TextDecoder();

        while (true) {
          const { done, value } = await reader.read();
          if (done) break;

          const chunk = decoder.decode(value);
          const lines = chunk.split('\\n');

          for (const line of lines) {
            if (line.startsWith('data: ')) {
              try {
                const data = JSON.parse(line.substring(6));
                if (data.text) {
                  accumulatedText += data.text;
                  bubble.innerHTML = formatMarkdown(accumulatedText);
                  scrollToBottom();
                }
                if (data.auto_rotated) {
                  appConfig.current_key = data.key_id;
                  renderKeyMatrix();
                  updateStatus();
                }
              } catch (err) {}
            }
          }
        }
      } catch (err) {
        bubble.innerHTML = `<span style="color:#ef4444;">Error: ${err.message}</span>`;
      } finally {
        isGenerating = false;
        document.getElementById('send-button').disabled = false;
      }
    }

    function appendMessage(role, text) {
      const container = document.getElementById('chat-messages');
      const row = document.createElement('div');
      row.className = `message-row ${role}`;
      
      const avatar = document.createElement('div');
      avatar.className = `avatar ${role}`;
      avatar.textContent = role === 'user' ? '👤' : '🤖';

      const bubble = document.createElement('div');
      bubble.className = 'bubble';
      bubble.innerHTML = formatMarkdown(text);

      if (role === 'user') {
        row.appendChild(bubble);
        row.appendChild(avatar);
      } else {
        row.appendChild(avatar);
        row.appendChild(bubble);
      }

      container.appendChild(row);
      scrollToBottom();
      return bubble;
    }

    function scrollToBottom() {
      const container = document.getElementById('chat-messages');
      container.scrollTop = container.scrollHeight;
    }

    async function resetChat() {
      await fetch('/api/reset', { method: 'POST' });
      document.getElementById('chat-messages').innerHTML = `
        <div class="welcome-hero" id="hero-banner">
          <div style="font-size: 36px;">🤖</div>
          <h2>Session Reset</h2>
          <p>Conversation history has been cleared. Start a fresh thread anytime.</p>
        </div>
      `;
    }

    function exportChat() {
      const messages = document.querySelectorAll('.message-row');
      let markdown = "# Gemini Studio Chat Export\\n\\n";
      messages.forEach(m => {
        const isUser = m.classList.contains('user');
        const text = m.querySelector('.bubble').innerText;
        markdown += `### ${isUser ? 'User' : 'Gemini'}\\n\\n${text}\\n\\n---\\n\\n`;
      });

      const blob = new Blob([markdown], { type: 'text/markdown' });
      const a = document.createElement('a');
      a.href = URL.createObjectURL(blob);
      a.download = `gemini-chat-${Date.now()}.md`;
      a.click();
    }

    // Auto resize textarea
    const textarea = document.getElementById('user-prompt');
    textarea.addEventListener('input', function() {
      this.style.height = 'auto';
      this.style.height = (this.scrollHeight) + 'px';
    });

    loadConfig();
  </script>
</body>
</html>
"""

class GeminiRequestHandler(BaseHTTPRequestHandler):
    def log_message(self, format, *args):
        # Silence verbose standard logging for cleaner terminal output
        return

    def do_GET(self):
        parsed = urlparse(self.path)
        if parsed.path == "/" or parsed.path == "/index.html":
            self.send_response(200)
            self.send_header("Content-Type", "text/html; charset=utf-8")
            self.end_headers()
            self.wfile.write(HTML_PAGE.encode("utf-8"))
        elif parsed.path == "/api/config":
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            payload = {
                "keys": {k: v[:6] + "..." + v[-4:] if len(v) > 10 else v for k, v in manager.keys.items()},
                "current_key": manager.current_key_id,
                "models": DEFAULT_MODELS,
                "current_model": manager.model,
                "search": manager.search_grounding,
                "personas": list(SYSTEM_PERSONAS.keys())
            }
            self.wfile.write(json.dumps(payload).encode("utf-8"))
        else:
            self.send_response(404)
            self.end_headers()

    def do_POST(self):
        parsed = urlparse(self.path)
        content_length = int(self.headers.get("Content-Length", 0))
        body = self.rfile.read(content_length).decode("utf-8") if content_length > 0 else "{}"
        data = json.loads(body) if body else {}

        if parsed.path == "/api/set_key":
            success = manager.set_key(data.get("key_id", ""))
            self.send_response(200 if success else 400)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            self.wfile.write(json.dumps({"status": "ok" if success else "failed"}).encode("utf-8"))

        elif parsed.path == "/api/set_model":
            manager.set_model(data.get("model", "gemini-2.5-flash"))
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            self.wfile.write(b'{"status":"ok"}')

        elif parsed.path == "/api/set_search":
            manager.set_search(data.get("search", False))
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            self.wfile.write(b'{"status":"ok"}')

        elif parsed.path == "/api/set_persona":
            key = data.get("persona_key", "general")
            manager.set_persona(SYSTEM_PERSONAS.get(key, SYSTEM_PERSONAS["general"]))
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            self.wfile.write(b'{"status":"ok"}')

        elif parsed.path == "/api/reset":
            manager.init_client()
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            self.wfile.write(b'{"status":"ok"}')

        elif parsed.path == "/api/chat_stream":
            prompt = data.get("message", "")
            self.send_response(200)
            self.send_header("Content-Type", "text/event-stream")
            self.send_header("Cache-Control", "no-cache")
            self.send_header("Connection", "keep-alive")
            self.end_headers()

            if not HAS_GENAI:
                err_msg = json.dumps({"text": "⚠️ 'google-genai' SDK is not installed. Run: pip install google-genai"})
                self.wfile.write(f"data: {err_msg}\n\n".encode("utf-8"))
                return

            try:
                # First attempt
                stream = manager.chat_session.send_message_stream(prompt)
                for chunk in stream:
                    if chunk.text:
                        line = json.dumps({"text": chunk.text})
                        self.wfile.write(f"data: {line}\n\n".encode("utf-8"))
                        self.wfile.flush()
            except Exception as e:
                err_str = str(e).lower()
                # Auto rotation logic for quota limits (429)
                if ("429" in err_str or "quota" in err_str or "exhausted" in err_str) and manager.auto_rotate:
                    new_key = manager.rotate_key()
                    notice = json.dumps({"text": f"\n\n⚡ *Quota hit on previous key. Auto-rotated to {new_key}! Retrying...*\n\n", "auto_rotated": True, "key_id": new_key})
                    self.wfile.write(f"data: {notice}\n\n".encode("utf-8"))
                    self.wfile.flush()

                    try:
                        retry_stream = manager.chat_session.send_message_stream(prompt)
                        for chunk in retry_stream:
                            if chunk.text:
                                line = json.dumps({"text": chunk.text})
                                self.wfile.write(f"data: {line}\n\n".encode("utf-8"))
                                self.wfile.flush()
                    except Exception as retry_err:
                        err_line = json.dumps({"text": f"\n\n❌ Error with new key: {retry_err}"})
                        self.wfile.write(f"data: {err_line}\n\n".encode("utf-8"))
                else:
                    err_line = json.dumps({"text": f"\n\n❌ Error: {e}"})
                    self.wfile.write(f"data: {err_line}\n\n".encode("utf-8"))

            self.wfile.write(b"data: {\"done\": true}\n\n")
            self.wfile.flush()
        else:
            self.send_response(404)
            self.end_headers()

def run_terminal_cli():
    """Terminal Mode Fallback when requested."""
    CYAN = "\033[96m"
    GREEN = "\033[92m"
    YELLOW = "\033[93m"
    RED = "\033[91m"
    BOLD = "\033[1m"
    RESET = "\033[0m"

    print(f"\n{BOLD}{CYAN}=================================================={RESET}")
    print(f"{BOLD}{CYAN}          🤖 GEMINI PRO TERMINAL AGENT            {RESET}")
    print(f"{BOLD}{CYAN}=================================================={RESET}")
    print(f"Active Key: {GREEN}{manager.current_key_id}{RESET} ({len(manager.keys)} total keys loaded)")
    print(f"Model:      {GREEN}{manager.model}{RESET}")
    print(f"Commands:   {YELLOW}/model, /key, /nextkey, /search, /reset, /exit{RESET}\n")

    while True:
        try:
            user_input = input(f"{BOLD}{GREEN}You > {RESET}").strip()
            if not user_input:
                continue

            if user_input.startswith("/"):
                cmd = user_input.split(maxsplit=1)
                action = cmd[0].lower()
                arg = cmd[1].strip() if len(cmd) > 1 else ""

                if action in ("/exit", "/quit"):
                    print(f"{CYAN}Agent: Goodbye!{RESET}")
                    break
                elif action == "/model":
                    if arg:
                        manager.set_model(arg)
                        print(f"{GREEN}✓ Model set to {arg}{RESET}\n")
                    else:
                        print(f"Current: {manager.model}\n")
                elif action == "/key":
                    if arg and manager.set_key(arg.upper()):
                        print(f"{GREEN}✓ Switched to {arg.upper()}{RESET}\n")
                    else:
                        print(f"Keys: {', '.join(manager.keys.keys())}\n")
                elif action == "/nextkey":
                    k = manager.rotate_key()
                    print(f"{YELLOW}⚡ Rotated to {k}{RESET}\n")
                elif action == "/reset":
                    manager.init_client()
                    print(f"{GREEN}✓ Conversation reset.{RESET}\n")
                continue

            print(f"\n{BOLD}{CYAN}Gemini > {RESET}", end="", flush=True)
            try:
                stream = manager.chat_session.send_message_stream(user_input)
                for chunk in stream:
                    if chunk.text:
                        print(chunk.text, end="", flush=True)
                print("\n")
            except Exception as e:
                print(f"\n{RED}Error: {e}{RESET}\n")

        except (KeyboardInterrupt, EOFError):
            print("\nExiting...")
            break

def find_available_port(start_port=5000):
    port = start_port
    while port < 6000:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            if s.connect_ex(("127.0.0.1", port)) != 0:
                return port
            port += 1
    return 5000

def main():
    if "--cli" in sys.argv:
        run_terminal_cli()
        return

    port = find_available_port(5000)
    server_address = ("127.0.0.1", port)
    httpd = HTTPServer(server_address, GeminiRequestHandler)

    url = f"http://127.0.0.1:{port}"
    print("\n" + "="*56)
    print(" 🚀  GEMINI UI PRO - WEB DASHBOARD READY")
    print("="*56)
    print(f" 🌐  URL:          \033[96m{url}\033[0m")
    print(f" 🔑  Loaded Keys:  {', '.join(manager.keys.keys())}")
    print(f" 🤖  Active Model: {manager.model}")
    print("="*56)
    print(" 💡  Tip: Pass '--cli' flag to run strictly in terminal mode.")
    print(" 🛑  Press Ctrl+C to terminate the server.\n")

    # Automatically launch browser in background
    threading.Timer(1.0, lambda: webbrowser.open(url)).start()

    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\nStopping Gemini UI Pro server...")
        httpd.server_close()

if __name__ == "__main__":
    main()
