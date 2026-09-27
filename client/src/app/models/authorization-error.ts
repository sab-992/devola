import { HttpErrorResponse } from "@angular/common/http";


export interface AuthorizationError {
    handler: () => void;
    error: HttpErrorResponse;
}