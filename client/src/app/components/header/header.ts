import { Component, inject } from '@angular/core';
import { ThemeComponent } from "@components/theme/theme";
import { RouterLink, RouterLinkActive } from "@angular/router";
import { UserService } from '@services/user/user';
import { routes } from '@config/app.routes';
import { MatButtonModule } from '@angular/material/button';
import { MatIconModule } from '@angular/material/icon';
import { MatMenuModule } from '@angular/material/menu';


@Component({
    selector: 'app-header',
    imports: [ThemeComponent, RouterLink, RouterLinkActive, MatButtonModule, MatIconModule, MatMenuModule],
    templateUrl: './header.html',
    styleUrl: './header.scss',
})
export class HeaderComponent {
    private readonly m_userService = inject(UserService)
    private readonly m_routes = routes.filter((route) => route.label && route.icon);

    constructor() {}

    public get routes() {
        return this.m_routes;
    }

    public logout(): void {
        this.m_userService.logout();
    }

    public buildPath(path: string) : string {
        return `/${path}`;
    }
}
