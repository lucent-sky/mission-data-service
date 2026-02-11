# Mission Data Service SRS

## 1. Purpose

The Mission Data Service (MDS) is a containerized C++ service responsible for receiving,
validating, storing, and summarizing mission data records transmitted over HTTP.
This project demonstrates structured software lifecycle development practices
consistent with defense industry standards.

---

## 2. Scope

The system provides:
- HTTP-based ingestion of structured mission data
- Validation of incoming data
- In-memory storage of validated records
- Statistical summary retrieval

The system is not intended for production deployment and does not implement
persistent storage, authentication, or encryption.

---

## 3. Definitions

- Record: A structured data object containing source ID, value, and timestamp.
- Valid Record: A record meeting all validation criteria defined in Section 5.

---

## 4. System Overview

The system shall operate as a standalone containerized microservice
accessible over HTTP on a configurable port.

---

## 5. Functional Requirements

### Ingestion

REQ-1: The system shall accept HTTP POST requests at endpoint `/ingest`.

REQ-2: The system shall accept JSON payloads with the following schema:

{
  "source_id": string,
  "value": number,
  "timestamp": unsigned integer
}

REQ-3: The system shall validate that all required fields are present.

REQ-4: The system shall reject malformed JSON with HTTP 400.

REQ-5: The system shall reject records with:
- Empty source_id
- Non-finite numeric values
- Timestamps equal to zero

REQ-6: The system shall store validated records in memory.

---

### Summary

REQ-7: The system shall provide HTTP GET endpoint `/stats`.

REQ-8: The `/stats` endpoint shall return:
- Total record count
- Minimum value
- Maximum value
- Average value

REQ-9: The system shall return HTTP 204 if no records are available.

---

### Operational

REQ-10: The system shall support configurable listening port via environment variable.

REQ-11: The system shall handle SIGTERM and exit gracefully.

REQ-12: The system shall run as a non-root user inside a container.

---

## 6. Non-Functional Requirements

NFR-1: The system shall be implemented in C++17.

NFR-2: The system shall compile without warnings using:
-Wall -Wextra -Wpedantic

NFR-3: The system shall be modular, separating:
- HTTP interface
- Validation logic
- Data storage

NFR-4: The system shall provide traceability between requirements and test cases.

---

## 7. Assumptions and Constraints

- In-memory storage only
- No persistence across restarts
- No authentication
- Single-node deployment

---

## 8. Future Enhancements

- TLS support
- Persistent storage
- Horizontal scaling
- Health check endpoint
