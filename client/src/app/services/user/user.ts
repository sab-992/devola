import { HttpErrorResponse } from '@angular/common/http';
import { inject, Service } from '@angular/core';
import { Router } from '@angular/router';
import { environment } from '@environments/environment';
import { Login } from '@models/login';
import { HttpService } from '@services/http/http';


type ErrorCallback = (error: HttpErrorResponse) => void;

@Service()
export class UserService {
    private readonly m_http: HttpService = inject(HttpService);
    private readonly m_router: Router = inject(Router);

    constructor() {}

    public login(loginInformation: Login, callback: ErrorCallback | undefined=undefined) {
        return this.m_http.post(this.buildPath("/login"), loginInformation).subscribe({ next: this.handleLogin.bind(this), error: callback });
    }

    private handleLogin() {
        this.m_router.navigate(['/feed']);
    }

    private buildPath(path: string) {
        return `${environment.apiUrl}/user${path}`;
    }
}