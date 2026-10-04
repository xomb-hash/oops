#!/usr/bin/env python3
import os
import re
import json
import urllib.request
import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText

# Configuration
REPO_URL = "https://raw.githubusercontent.com/sndsh404/summer-2027-internships/main/README.md"
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
DB_FILE = os.path.join(SCRIPT_DIR, "seen_jobs.json")
REPORT_FILE = os.path.join(SCRIPT_DIR, "latest_internships.md")
ALL_JOBS_FILE = os.path.join(SCRIPT_DIR, "all_internships.md")

# Known MNC keywords (to differentiate from startups)
MNC_KEYWORDS = {
    "jpmorgan", "chase", "bank of america", "amazon", "citi", "blackrock", "tsmc", 
    "delta", "ge aerospace", "ge appliances", "blue origin", "optiver", "imc", 
    "two sigma", "d. e. shaw", "tower research", "susquehanna", "virtu", "dtcc", 
    "arrowstreet", "flow traders", "caterpillar", "liveramp", "bytedance", "rippling"
}

def fetch_readme(url):
    """Fetches the markdown content of the README file."""
    try:
        req = urllib.request.Request(
            url, 
            headers={'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64)'}
        )
        with urllib.request.urlopen(req) as response:
            return response.read().decode('utf-8')
    except Exception as e:
        print(f"Error fetching README: {e}")
        return None

def parse_jobs(markdown_content):
    """Parses markdown tables to extract job details."""
    jobs = []
    # Regex to find markdown table rows
    # Example row: | Company | Role | Location | [apply](link) | Date |
    row_pattern = re.compile(r'^\|\s*([^|]+?)\s*\|\s*([^|]+?)\s*\|\s*([^|]+?)\s*\|\s*\[apply\]\((http[s]?://[^\)]+)\)\s*\|\s*([^|]*?)\s*\|$', re.MULTILINE)
    
    matches = row_pattern.findall(markdown_content)
    for match in matches:
        company = match[0].strip()
        role = match[1].strip()
        location = match[2].strip()
        link = match[3].strip()
        date_added = match[4].strip()
        
        # Skip header/separator rows
        if company.lower() == "company" or "---" in company:
            continue
            
        # Determine category (MNC vs Startup)
        is_mnc = False
        comp_lower = company.lower()
        for kw in MNC_KEYWORDS:
            if kw in comp_lower:
                is_mnc = True
                break
        
        # Additional startup detection (e.g. YCombinator links)
        is_startup = not is_mnc or "ycombinator" in link.lower()
        category = "MNC" if is_mnc else "Startup"
        
        jobs.append({
            "company": company,
            "role": role,
            "location": location,
            "link": link,
            "date": date_added if date_added else "N/A",
            "category": category
        })
    return jobs

def load_seen_jobs():
    """Loads already seen jobs from local DB."""
    if os.path.exists(DB_FILE):
        try:
            with open(DB_FILE, "r") as f:
                return set(json.load(f))
        except Exception:
            return set()
    return set()

def save_seen_jobs(seen_links):
    """Saves seen jobs to local DB."""
    with open(DB_FILE, "w") as f:
        json.dump(list(seen_links), f, indent=4)

def load_env_variables():
    """Tries to load SMTP config from a .env file or environment variables."""
    env = {}
    env_file = os.path.join(SCRIPT_DIR, ".env")
    if os.path.exists(env_file):
        with open(env_file, "r") as f:
            for line in f:
                if "=" in line and not line.strip().startswith("#"):
                    k, v = line.strip().split("=", 1)
                    env[k.strip()] = v.strip().strip('"').strip("'")
    
    # Fallback to system environment variables
    config = {
        "SMTP_HOST": env.get("SMTP_HOST") or os.getenv("SMTP_HOST"),
        "SMTP_PORT": env.get("SMTP_PORT") or os.getenv("SMTP_PORT"),
        "SMTP_USER": env.get("SMTP_USER") or os.getenv("SMTP_USER"),
        "SMTP_PASS": env.get("SMTP_PASS") or os.getenv("SMTP_PASS"),
        "SENDER_EMAIL": env.get("SENDER_EMAIL") or os.getenv("SENDER_EMAIL"),
        "RECEIVER_EMAIL": env.get("RECEIVER_EMAIL") or os.getenv("RECEIVER_EMAIL"),
    }
    return config

def send_email(config, new_jobs):
    """Sends email report if SMTP credentials are provided."""
    if not (config["SMTP_HOST"] and config["SMTP_USER"] and config["SMTP_PASS"] and config["RECEIVER_EMAIL"]):
        print("SMTP credentials not fully configured. Skipping email notification.")
        return False
        
    sender = config["SENDER_EMAIL"] or config["SMTP_USER"]
    receiver = config["RECEIVER_EMAIL"]
    
    msg = MIMEMultipart('alternative')
    msg['Subject'] = f"🔔 {len(new_jobs)} New Summer 2027 Internship Opportunities Found!"
    msg['From'] = sender
    msg['To'] = receiver
    
    # Generate HTML content
    html_content = """
    <html>
    <head>
        <style>
            body { font-family: Arial, sans-serif; line-height: 1.6; color: #333333; }
            h2 { color: #1a73e8; }
            table { width: 100%; border-collapse: collapse; margin-top: 15px; }
            th, td { padding: 10px; border: 1px solid #dddddd; text-align: left; }
            th { background-color: #f2f2f2; }
            .badge { padding: 3px 8px; border-radius: 4px; font-size: 12px; font-weight: bold; color: white; }
            .mnc { background-color: #1a73e8; }
            .startup { background-color: #34a853; }
            .apply-btn { display: inline-block; padding: 6px 12px; background-color: #ea4335; color: white !important; text-decoration: none; border-radius: 4px; font-weight: bold; }
        </style>
    </head>
    <body>
        <h2>New Internship Postings Detected</h2>
        <p>Here are the latest Summer 2027 paid internship roles from top MNCs and Startups:</p>
        <table>
            <thead>
                <tr>
                    <th>Company</th>
                    <th>Role</th>
                    <th>Location</th>
                    <th>Category</th>
                    <th>Action</th>
                </tr>
            </thead>
            <tbody>
    """
    
    for job in new_jobs:
        badge_class = "mnc" if job["category"] == "MNC" else "startup"
        html_content += f"""
            <tr>
                <td><strong>{job['company']}</strong></td>
                <td>{job['role']}</td>
                <td>{job['location']}</td>
                <td><span class="badge {badge_class}">{job['category']}</span></td>
                <td><a class="apply-btn" href="{job['link']}">Apply Now</a></td>
            </tr>
        """
        
    html_content += """
            </tbody>
        </table>
        <br>
        <p>Best regards,<br>Your Autonomous Internship Agent 🚀</p>
    </body>
    </html>
    """
    
    msg.attach(MIMEText(html_content, 'html'))
    
    try:
        port = int(config["SMTP_PORT"]) if config["SMTP_PORT"] else 587
        server = smtplib.SMTP(config["SMTP_HOST"], port)
        server.starttls()
        server.login(config["SMTP_USER"], config["SMTP_PASS"])
        server.sendmail(sender, receiver, msg.as_string())
        server.quit()
        print("Email report sent successfully!")
        return True
    except Exception as e:
        print(f"Failed to send email: {e}")
        return False

def generate_reports(all_jobs, new_jobs):
    """Writes files containing job listings."""
    # Write all jobs
    with open(ALL_JOBS_FILE, "w") as f:
        f.write("# All Summer 2027 Tech Internships\n\n")
        f.write("| Company | Role | Location | Category | Link | Added |\n")
        f.write("| --- | --- | --- | --- | --- | --- |\n")
        for job in all_jobs:
            f.write(f"| {job['company']} | {job['role']} | {job['location']} | {job['category']} | [Link]({job['link']}) | {job['date']} |\n")
            
    # Write latest jobs
    with open(REPORT_FILE, "w") as f:
        f.write(f"# Latest Summer 2027 Internship Postings ({len(new_jobs)} New)\n\n")
        if new_jobs:
            f.write("| Company | Role | Location | Category | Link | Added |\n")
            f.write("| --- | --- | --- | --- | --- | --- |\n")
            for job in new_jobs:
                f.write(f"| {job['company']} | {job['role']} | {job['location']} | {job['category']} | [Link]({job['link']}) | {job['date']} |\n")
        else:
            f.write("No new internship postings detected since last run.\n")

def main():
    print("Starting Summer 2027 Internship Agent...")
    markdown = fetch_readme(REPO_URL)
    if not markdown:
        print("Aborting.")
        return
        
    jobs = parse_jobs(markdown)
    print(f"Total jobs parsed from repository: {len(jobs)}")
    
    seen_links = load_seen_jobs()
    
    new_jobs = []
    for job in jobs:
        if job["link"] not in seen_links:
            new_jobs.append(job)
            seen_links.add(job["link"])
            
    print(f"New jobs detected: {len(new_jobs)}")
    
    # Save the updated DB
    save_seen_jobs(seen_links)
    
    # Generate reports
    generate_reports(jobs, new_jobs)
    print(f"Reports generated: {REPORT_FILE} and {ALL_JOBS_FILE}")
    
    # Try sending email
    if new_jobs:
        config = load_env_variables()
        send_email(config, new_jobs)
    else:
        print("No new jobs to notify about.")

if __name__ == "__main__":
    main()
