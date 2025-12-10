# webserv

webserv is a minimal C/C++ web server, inspired by Nginx. It implements the core functionality of a web server, including handling HTTP requests, serving static content, executing CGI scripts, and providing configurable error and index pages.

This project demonstrates a lightweight implementation of a web server, with a simple build system via Makefile and organized directories for source code, configuration files, test assets, and templates.

---

## Features

-   **HTTP Request Handling:** Supports GET, POST, and DELETE requests.
-   **Static File Serving:** Serves files from configured document roots (HTML, CSS, JS, images, etc.).
-   **CGI Support:** Executes CGI scripts and returns dynamic content.
-   **Directory Indexing:** Generates directory listings or uses index templates from `indexes/` when no index file is present.
-   **Multiple Configuration Support:** Reads configuration files from `confFiles/` to define:
    -   Ports to listen on
    -   Document roots
    -   Error pages
    -   Index files
    -   Allowed HTTP methods
-   **Basic Logging:** Logs requests and responses.
-   **Concurrent Connections:** Handles multiple client connections via select.
-   **Signal Handling:** Gracefully handles termination signals (e.g., SIGINT).

---

## Tech Stack

-   Language: C/C++
-   Build: Make
-   Networking: POSIX/BSD sockets
-   Formatting: clang-format

---

## Build & Run

```bash
git clone https://github.com/procrastinator-101/webserv.git
cd webserv
make
./webserv confFiles/example.conf
```

## Usage

After building the server, you can run it with a configuration file:

```bash
# Start the server
./webserv confFiles/example.conf
```


## Testing webserv in a Browser

Follow these steps to test your webserv web server using a browser:

### 1. Start the Server

Make sure your server is built and run it with a configuration file:

```bash
./webserv confFiles/example.conf
```

Ensure the configuration file specifies a document root (folder with your HTML, CSS, JS files).
Ensure the configuration specifies the port (default: 8080, or your custom port).

### 2. Open a Browser

Launch any web browser (Chrome, Firefox, Edge, etc.).
Enter the server URL in the address bar:

```
http://localhost:8080/
```

Replace 8080 with your configured port if different.

### 3. Access Static Content

Place index.html or other static files in the document root.
Navigate to them in the browser:

```
http://localhost:8080/index.html
http://localhost:8080/css/style.css
http://localhost:8080/images/logo.png
```

Existing files should be served correctly.
Missing files will return the custom error page (from error_pages/), if configured.

### 4. Test Directory Indexing

Navigate to a directory without an index file:

```
http://localhost:8080/test_folder/
```

webserv should either:
- Show a directory listing using indexes/ templates, or
- Return a 403/404 error page if directory indexing is disabled (default behavior).

### 5. Test CGI Scripts

Place a CGI script in your configured CGI folder (e.g., cgi-bin/).
Navigate to the CGI URL:

```
http://localhost:8080/cgi-bin/script.cgi
```

webserv will execute the script and display the output.

### 6. Stop the Server

Press Ctrl+C in the terminal where webserv is running to stop it gracefully.